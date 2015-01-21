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
QVariant operation(QVariant a, QVariant b, OpFn op)
{
  try {
    return QVariant::fromValue(op(expression(a), expression(b)));
  } catch (const NonExpressionError&) {
    qWarning() << "No expression!";
    return QVariant {};
  }
}

} // anonymous namespace

QVariant Constraint::plus(QVariant a, QVariant b)
{
  return operation(a, b, ableton::estd::plus<>{});
}

QVariant Constraint::minus(QVariant a, QVariant b)
{
  return operation(a, b, ableton::estd::minus<>{});
}

QVariant Constraint::times(QVariant a, QVariant b)
{
  return operation(a, b, ableton::estd::multiplies<>{});
}

QVariant Constraint::divide(QVariant a, QVariant b)
{
  return operation(a, b, ableton::estd::divides<>{});
}

QVariant Constraint::eq(QVariant a, QVariant b)
{
  return operation(a, b, ableton::estd::equal_to<>{});
}

QVariant Constraint::geq(QVariant a, QVariant b)
{
  return operation(a, b, ableton::estd::greater_equal<>{});
}

QVariant Constraint::leq(QVariant a, QVariant b)
{
  return operation(a, b, ableton::estd::less_equal<>{});
}

} // namespace cassowary
} // namespace aqt
