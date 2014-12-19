// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#include <ableton/cassowary/Constraint.hpp>
#include <ableton/cassowary/Variable.hpp>
#include <ableton/cassowary/Strength.hpp>
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

namespace ableton {
namespace cassowary {

namespace {

struct WithQVariantError : std::runtime_error
{
  using std::runtime_error::runtime_error;
};

template <typename ...Ts>
struct WithQVariantImpl;

template <typename T, typename... Ts>
struct WithQVariantImpl<T, Ts...>
{
  template <typename VisitorT>
  void operator() (const QVariant& var, VisitorT&& visitor)
  {
    if (var.canConvert<T>()) {
      return std::forward<VisitorT>(visitor)(var.value<T>());
    } else {
      auto impl = WithQVariantImpl<Ts...> {};
      impl(var, visitor);
    }
  }

  template <typename VisitorT>
  void operator() (QVariant& var, VisitorT&& visitor)
  {
    if (var.canConvert<T>()) {
      std::forward<VisitorT>(visitor)(var.value<T>());
    } else {
      auto impl = WithQVariantImpl<Ts...> {};
      impl(var, visitor);
    }
  }
};

template <>
struct WithQVariantImpl<>
{
  template <typename VisitorT>
  void operator() (const QVariant& var, VisitorT&& visitor)
  {
    Q_UNUSED(var);
    Q_UNUSED(visitor);
    throw WithQVariantError { "No appropiate type" };
  }
};

//!
// @todo This implementation is acknowledged to not be
// standards-compliant but it is the simplest.  A better
// implementation will eventually come to our libraries, usable also
// with ableton::base::match()
//
template <typename... Fns>
struct WithQVariantVisitor : Fns...
{
  template <typename ...Fns2>
  WithQVariantVisitor(Fns2&& ...fns)
    : Fns(std::forward<Fns2>(fns))...
  {}
};

//!
// @todo This is a sketch of a potential real tool that could go in
// ableton::base. Eventually we would want to use mpl sequences
// (including) ableton::meta::Pack for types and improve the semantics
// a little bit.  For example, it would be nice to have return type
// deduction like ableton::base::Variant.
//
template <typename ...Types, typename ...Fns>
void withQVariant(QVariant& var, Fns&& ...fns)
{
  auto impl = WithQVariantImpl<Types...> {};
  auto visitor = WithQVariantVisitor<Fns...> { fns... };
  impl(var, visitor);
}

template <typename ...Types, typename ...Fns>
void withQVariant(const QVariant& var, Fns&& ...fns)
{
  auto impl = WithQVariantImpl<Types...> {};
  auto visitor = WithQVariantVisitor<Fns...> { fns... };
  impl(var, visitor);
}

} // anonymous namespace

struct Constraint::Setter
{
  Constraint& target;

  template <typename T>
  void operator() (T&& origin)
  {
    return target.set(
      std::make_shared<typename std::decay<T>::type>(
        std::forward<T>(origin)));
  }
};

Constraint::Constraint(QQuickItem* pParent)
  : ConstraintBase(pParent)
{
  connect(this, &Constraint::exprChanged, [this](QVariant expr) {
    try {
      withQVariant<
        rhea::linear_inequality,
        rhea::linear_equation>(
          expr,
          Setter { *this });
    } catch (const WithQVariantError&) {}
  });
}

namespace {

struct NonExpressionError : std::runtime_error
{
  using std::runtime_error::runtime_error;
};

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
  throw NonExpressionError { "Invalid argument" };
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
  return operation(a, b, estd::plus<>{});
}

QVariant Constraint::minus(QVariant a, QVariant b)
{
  return operation(a, b, estd::minus<>{});
}

QVariant Constraint::times(QVariant a, QVariant b)
{
  return operation(a, b, estd::multiplies<>{});
}

QVariant Constraint::divide(QVariant a, QVariant b)
{
  return operation(a, b, estd::divides<>{});
}

QVariant Constraint::eq(QVariant a, QVariant b)
{
  return operation(a, b, estd::equal_to<>{});
}

QVariant Constraint::geq(QVariant a, QVariant b)
{
  return operation(a, b, estd::greater_equal<>{});
}

QVariant Constraint::leq(QVariant a, QVariant b)
{
  return operation(a, b, estd::less_equal<>{});
}

} // namespace cassowary
} // namespace ableton
