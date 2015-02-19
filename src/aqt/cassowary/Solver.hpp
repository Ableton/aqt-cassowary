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

#include <aqt/cassowary/Contextual.hpp>
#include <ableton/build_system/Warnings.hpp>

namespace aqt {
namespace cassowary {

/*!
 *  Provides a context to register and resolve constraints.
 *
 *  It may as well simply extend another context, following the normal
 *  rules of `Contextuals`.
 *
 *  Note that the solver does not resolve the equations until the next
 *  redraw of the screen.  This prevents intermediate values to confuse
 *  the solver during binding propagations, as well as improves
 *  performance as only the minimal set of updates to visualize new
 *  layouts is performed.  If needed (e.g. in tests) one may manually
 *  update the system with the `commit()` method.
 *
 *  @par Example
 *  @code
 *  Solver {}
 *  @endcode
 *
 *  @par Import in QML
 *  `import Aqt.Cassowary 1.0`
 *
 *  @since 1.0
 *  @see Contextual
 */
class Solver : public Contextual
{
  Q_OBJECT

public:
  Solver(QQuickItem* pParent = nullptr);
  ~Solver() override;

  /*!
   *  Toggles debug traces for the context provided by this solver.
   *  This has no effect if the solver is simply extending another
   *  context. Also:
   *
   *    - The debug traces may are still affected by the general Qt
   *      logging filters.
   *
   *    - Debug traces will never be shown if NDEBUG is defined at
   *      compilation time.
   */
  Q_PROPERTY(bool debug MEMBER mDebug NOTIFY debugChanged)
  Q_SIGNAL void debugChanged(bool debug);

protected:
  void updatePolish() override;
  void componentComplete() override;
  std::shared_ptr<Context> provided() override;
  void addIn(Context&) override {}
  void removeIn(Context&) override {}

private:
  bool mDebug = false;
  std::shared_ptr<Context> mProvided;
};

} // namespace cassowary
} // namespace aqt
