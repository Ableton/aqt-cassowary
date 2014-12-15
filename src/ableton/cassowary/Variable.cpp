// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#include <ableton/cassowary/Variable.hpp>

namespace ableton {
namespace cassowary {

Variable::Variable(QQuickItem* pParent)
  : SolverItem(pParent)
{
}

double Variable::value() const
{
  return mVariable.value();
}

void Variable::setValue(double value)
{
  mVariable.set_value(value);
  Q_EMIT valueChanged(value);
}

const rhea::variable& Variable::variableImpl() const
{
  return mVariable;
}

void Variable::addIn(Context& ctx)
{
  ctx.setVariableCallback(
    mVariable,
    [this](const rhea::variable&, rhea::simplex_solver&) {
      Q_EMIT valueChanged(value());
    });
}

void Variable::removeIn(Context& ctx)
{
  ctx.setVariableCallback(mVariable, nullptr);
}

} // namespace cassowary
} // namespace ableton
