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

#include <aqt/cassowary/Constraint.hpp>
#include <aqt/cassowary/Variable.hpp>
#include <aqt/cassowary/Strength.hpp>
#include <ableton/estd/functional.hpp>
#include <ableton/build_system/Warnings.hpp>

ABL_DISABLE_WARNINGS
#include <rhea/linear_equation.hpp>
#include <rhea/linear_inequality.hpp>
#include <rhea/iostream.hpp>
ABL_RESTORE_WARNINGS

Q_DECLARE_METATYPE(rhea::linear_expression)
Q_DECLARE_METATYPE(rhea::linear_equation)
Q_DECLARE_METATYPE(rhea::linear_inequality)

namespace aqt {
namespace cassowary {

Constraint::Constraint(QQuickItem* pParent)
  : ConstraintBase(pParent)
{
  connect(this, &Constraint::exprChanged, [this](QVariant expr) {
    if (expr.canConvert<rhea::linear_inequality>()) {
      set(std::make_shared<rhea::linear_inequality>(
            expr.value<rhea::linear_inequality>()));
    }
    else if (expr.canConvert<rhea::linear_equation>()) {
      set(std::make_shared<rhea::linear_equation>(
            expr.value<rhea::linear_equation>()));
    } else {
      log("Empty expression set");
    }
  });
}

namespace {

struct NonExpressionError : std::exception {};

rhea::linear_expression expression(QVariant v)
{
  if (v.canConvert<rhea::linear_expression>()) {
    return v.value<rhea::linear_expression>();
  }
  if (v.canConvert<double>()) {
    return v.value<double>();
  }
  if (Variable* e = v.value<Variable*>()) {
    return e->variableImpl();
  }
  throw NonExpressionError{};
}

template <typename OpFn>
QVariant operation(OpFn op, QVariant a, QVariant b)
{
  try {
    return QVariant::fromValue(
      op(expression(b),
         expression(a)));
  } catch (const NonExpressionError&) {
    return QVariant {};
  }
}

template <typename OpFn, typename ...More>
QVariant operation(OpFn op, QVariant a, More&&... s)
{
  try {
    return QVariant::fromValue(
      op(expression(operation(op, std::forward<More>(s)...)),
         expression(a)));
  } catch (const NonExpressionError&) {
    return QVariant {};
  }
}

} // anonymous namespace

QVariant Constraint::plus(QVariant a, QVariant b)
{
  return operation(ableton::estd::plus<>{}, b, a);
}

QVariant Constraint::plus(QVariant a, QVariant b, QVariant c)
{
  return operation(ableton::estd::plus<>{}, c, b, a);
}

QVariant Constraint::plus(QVariant a, QVariant b, QVariant c, QVariant d)
{
  return operation(ableton::estd::plus<>{}, d, c, b, a);
}

QVariant Constraint::plus(QVariant a, QVariant b, QVariant c, QVariant d, QVariant e)
{
  return operation(ableton::estd::plus<>{}, e, d, c, b, a);
}

QVariant Constraint::plus(QVariant a, QVariant b, QVariant c, QVariant d, QVariant e,  QVariant f)
{
  return operation(ableton::estd::plus<>{}, f, e, d, c, b, a);
}

QVariant Constraint::minus(QVariant a, QVariant b)
{
  return operation(ableton::estd::minus<>{}, b, a);
}

QVariant Constraint::minus(QVariant a, QVariant b, QVariant c)
{
  return operation(ableton::estd::minus<>{}, c, b, a);
}

QVariant Constraint::minus(QVariant a, QVariant b, QVariant c, QVariant d)
{
  return operation(ableton::estd::minus<>{}, d, c, b, a);
}

QVariant Constraint::minus(QVariant a, QVariant b, QVariant c, QVariant d, QVariant e)
{
  return operation(ableton::estd::minus<>{}, e, d, c, b, a);
}

QVariant Constraint::minus(QVariant a, QVariant b, QVariant c, QVariant d, QVariant e,  QVariant f)
{
  return operation(ableton::estd::minus<>{}, f, e, d, c, b, a);
}

QVariant Constraint::times(QVariant a, QVariant b)
{
  return operation(ableton::estd::multiplies<>{}, b, a);
}

QVariant Constraint::times(QVariant a, QVariant b, QVariant c)
{
  return operation(ableton::estd::multiplies<>{}, c, b, a);
}

QVariant Constraint::times(QVariant a, QVariant b, QVariant c, QVariant d)
{
  return operation(ableton::estd::multiplies<>{}, d, c, b, a);
}

QVariant Constraint::times(QVariant a, QVariant b, QVariant c, QVariant d, QVariant e)
{
  return operation(ableton::estd::multiplies<>{}, e, d, c, b, a);
}

QVariant Constraint::times(QVariant a, QVariant b, QVariant c, QVariant d, QVariant e,  QVariant f)
{
  return operation(ableton::estd::multiplies<>{}, f, e, d, c, b, a);
}

QVariant Constraint::divide(QVariant a, QVariant b)
{
  return operation(ableton::estd::divides<>{}, b, a);
}

QVariant Constraint::divide(QVariant a, QVariant b, QVariant c)
{
  return operation(ableton::estd::divides<>{}, c, b, a);
}

QVariant Constraint::divide(QVariant a, QVariant b, QVariant c, QVariant d)
{
  return operation(ableton::estd::divides<>{}, d, c, b, a);
}

QVariant Constraint::divide(QVariant a, QVariant b, QVariant c, QVariant d, QVariant e)
{
  return operation(ableton::estd::divides<>{}, e, d, c, b, a);
}

QVariant Constraint::divide(QVariant a, QVariant b, QVariant c, QVariant d, QVariant e,  QVariant f)
{
  return operation(ableton::estd::divides<>{}, f, e, d, c, b, a);
}

QVariant Constraint::eq(QVariant a, QVariant b)
{
  return operation(ableton::estd::equal_to<>{}, b, a);
}

QVariant Constraint::geq(QVariant a, QVariant b)
{
  return operation(ableton::estd::greater_equal<>{}, b, a);
}

QVariant Constraint::leq(QVariant a, QVariant b)
{
  return operation(ableton::estd::less_equal<>{}, b, a);
}

} // namespace cassowary
} // namespace aqt
