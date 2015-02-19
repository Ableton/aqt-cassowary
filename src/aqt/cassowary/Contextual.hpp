// Copyright (c) 2014, 2015 Ableton AG, Berlin
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#pragma once

#include <aqt/cassowary/Context.hpp>
#include <ableton/build_system/Warnings.hpp>

ABL_DISABLE_WARNINGS
#include <QtQuick/QQuickItem>
#include <QtCore/QPointer>
#include <memory>
ABL_RESTORE_WARNINGS

namespace aqt {
namespace cassowary {

/*!
 *  Base class for items that need to be registered in a solver.
 *
 *  Most *Aqt.Cassowary* entities need to be registered in a solver to
 *  be effective.  This solver is sometimes referred to as the
 *  *context*.  The context in which an item will be determined:
 *
 *    - If the `extend` property is set, it will use the context of
 *      the entity it refers to.
 *
 *    - Otherwise, if the parent is also a contextual, it will use its
 *      context.
 *
 *  The second rule allows to conveniently define constraints directly
 *  in a context, and the previous allows for composition.
 *
 *  @par Example
 *  @code
 *  Solver {
 *      id: solver
 *      Variable { id: something }
 *      Constraint { expr: eq(something, 42) }
 *      Repeater {
 *          model: 4
 *          Solver {
 *              extend: solver
 *              Variable { id: other }
 *              Constraint { expr: geq(other, something) }
 *          }
 *      }
 *  }
 *  @endcode
 *
 *  @par Import in QML
 *  `import Aqt.Cassowary 1.0`
 *
 *  @since 1.0
 *  @see Contextual
 */
class Contextual : public QQuickItem
{
  Q_OBJECT

public:
  Contextual(QQuickItem* pParent = nullptr);

  std::shared_ptr<Context> context();
  std::shared_ptr<const Context> context() const;
  Q_SIGNAL void contextChanged();

  /*!
   *  If non-null, this contextual will use the same context as
   *  used by this contextual.
   */
  Q_PROPERTY(aqt::cassowary::Contextual* extend
             MEMBER mExtend NOTIFY extendChanged)
  Q_SIGNAL void extendChanged(aqt::cassowary::Contextual* extend);

  /*!
   *  Applies all the changes associated to the context used by this
   *  contextual.
   *
   *  @see Solver for more details on how and when are the changes
   *       usually applied.
   */
  Q_INVOKABLE void commit();

  /*!
   *  Executes the callback `cb` at the end of the next phase of
   *  updates in the solver, but before the changes are propagated to
   *  the bindings.
   */
  Q_INVOKABLE void defer(QJSValue cb);
  void defer(Context::Callback cb);

  // Used to disambiguate some calls because VS03 is dumb
  void defer_(Context::Callback cb) { defer(std::move(cb)); }

  /*!
   *  Used to log a debug trace, that may be visible when the current
   *  context's debug property is set.  Spaces will be printed between
   *  the arguments.  Arguments should be compatible with
   *  `std::ostream`.
   *
   *  @see Solver for more details on when debug output is visible.
   */
  template <typename ...Args>
  void log(Args&&... args) {
    auto ctx = context();
    if (ctx) {
      ctx->log(std::forward<Args>(args)...);
    }
  }

protected:
  void add();
  void remove();

  template <typename Fn>
  void update(Fn&& cb)
  {
    remove();
    std::forward<Fn>(cb)();
    add();
  }

  template <typename Fn>
  Context::Callback guarded(Fn fn)
  {
    auto guard = QPointer<QObject>{ this };
    return [guard, fn] {
      if (guard) {
        fn();
      }
    };
  }

  void updateContext();
  Contextual* provider() { return mProvider; }

  virtual std::shared_ptr<Context> provided() { return nullptr; }
  virtual void addIn(Context& impl) = 0;
  virtual void removeIn(Context& impl) = 0;

private:
  QPointer<Contextual> mExtend;
  QPointer<Contextual> mProvider;
  std::shared_ptr<Context> mContext;
};

} // namespace cassowary
} // namespace aqt
