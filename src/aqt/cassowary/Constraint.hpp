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

#include <aqt/cassowary/ConstraintBase.hpp>

namespace aqt {
namespace cassowary {

/*!
 *  @ingroup qml
 *  A linear constraint.
 *
 *  A linear constraint allows us to set equations or inequalities
 *  that relate variables in the system.   The solver will log errors
 *  in the presence of non-linear constraints or conflicting
 *  constraints with `Required` *strength*.
 *
 *  @par Example
 *  @code
 *  Solver {
 *      Variable { id: vX }
 *      Variable { id: vY }
 *      Constraint {
 *          expr: eq(vX, times(2, vY))
 *      }
 *      Constraint {
 *          expr: geq(plus(vX, vY), 42)
 *          when: root.height > 100
 *      }
 *      // Error, non-linear expression
 *      Constraint { expr: geq(times(vX, vX), vY) }
 *  }
 *  @endcode
 *
 *  @par Import in QML
 *  `import Aqt.Cassowary 1.0`
 *
 *  @since 1.0
 *  @see ConstraintBase
 *  @see Strength
 */
class Constraint : public ConstraintBase
{
  Q_OBJECT

public:
  Constraint(QQuickItem* pParent = nullptr);

  /*!
   *  Expression that determines the linear constraint.
   *  It can hold a *comparison expression* or `undefined`.  If
   *  `undefined`, the constraint will have no effect.
   */
  Q_PROPERTY(QVariant expr MEMBER mExpr NOTIFY exprChanged)
  Q_SIGNAL void exprChanged(QVariant expr);

  /*!
   *  Returns an *arithmetic expression* representing the sum of its
   *  arguments, or `undefined` if any of its arguments is
   *  `undefined`.  Its arguments may be *arithmetic expressions*,
   *  `Variable` objects or numbers.
   */
  Q_INVOKABLE QVariant plus(QVariant, QVariant);
  Q_INVOKABLE QVariant plus(QVariant, QVariant, QVariant);
  Q_INVOKABLE QVariant plus(QVariant, QVariant, QVariant, QVariant);
  Q_INVOKABLE QVariant plus(QVariant, QVariant, QVariant, QVariant, QVariant);
  Q_INVOKABLE QVariant plus(QVariant, QVariant, QVariant, QVariant, QVariant, QVariant);

  /*!
   *  Returns an *arithmetic expression* representing the substraction
   *  of its arguments, or `undefined` if any of its arguments is
   *  `undefined`.  Its arguments may be *arithmetic expressions*,
   *  `Variable` objects or numbers.
   */
  Q_INVOKABLE QVariant minus(QVariant, QVariant);
  Q_INVOKABLE QVariant minus(QVariant, QVariant, QVariant);
  Q_INVOKABLE QVariant minus(QVariant, QVariant, QVariant, QVariant);
  Q_INVOKABLE QVariant minus(QVariant, QVariant, QVariant, QVariant, QVariant);
  Q_INVOKABLE QVariant minus(QVariant, QVariant, QVariant, QVariant, QVariant, QVariant);

  /*!
   *  Returns an *arithmetic expression* representing the product of
   *  its arguments, or `undefined` if any of its arguments is
   *  `undefined`.  Its arguments may be *arithmetic expressions*,
   *  `Variable` objects or numbers.
   */
  Q_INVOKABLE QVariant times(QVariant, QVariant);
  Q_INVOKABLE QVariant times(QVariant, QVariant, QVariant);
  Q_INVOKABLE QVariant times(QVariant, QVariant, QVariant, QVariant);
  Q_INVOKABLE QVariant times(QVariant, QVariant, QVariant, QVariant, QVariant);
  Q_INVOKABLE QVariant times(QVariant, QVariant, QVariant, QVariant, QVariant, QVariant);

  /*!
   *  Returns an *arithmetic expression* representing the division of its
   *  arguments, or `undefined` if any of its arguments is
   *  `undefined`.  Its arguments may be *arithmetic expressions*,
   *  `Variable` objects or numbers.
   */
  Q_INVOKABLE QVariant divide(QVariant, QVariant);
  Q_INVOKABLE QVariant divide(QVariant, QVariant, QVariant);
  Q_INVOKABLE QVariant divide(QVariant, QVariant, QVariant, QVariant);
  Q_INVOKABLE QVariant divide(QVariant, QVariant, QVariant, QVariant, QVariant);
  Q_INVOKABLE QVariant divide(QVariant, QVariant, QVariant, QVariant, QVariant, QVariant);

  /*!
   *  Returns a *comparison expression* stating the equality of its
   *  arguments, or `undefined` if any of its arguments is
   *  `undefined`.  Its arguments may be *arithmetic expressions*,
   *  `Variable` objects or numbers.
   */
  Q_INVOKABLE QVariant eq(QVariant a, QVariant b);

  /*!
   *  Returns a *comparison expression* stating that its first
   *  argument should be less than the later, or `undefined` if any of
   *  its arguments is `undefined`.  Its arguments may be *arithmetic
   *  expressions*, `Variable` objects or numbers.
   */
  Q_INVOKABLE QVariant geq(QVariant a, QVariant b);

  /*!
   *  Returns a *comparison expression* stating that its first
   *  argument should be greater than the later, or `undefined` if any
   *  of its arguments is `undefined`.  Its arguments may be
   *  *arithmetic expressions*, `Variable` objects or numbers.
   */
  Q_INVOKABLE QVariant leq(QVariant a, QVariant b);

private:
  QVariant mExpr;
};

} // namespace cassowary
} // namespace aqt
