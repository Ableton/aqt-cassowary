// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#include <ableton/cassowary/Variable.hpp>

namespace ableton {
namespace cassowary {

Variable::Variable(QQuickItem* pParent)
  : SolverItem(pParent)
{
  connect(this, &Variable::initialChanged, [this] (double initial) {
    auto ctx = context();
    if (ctx && ctx->solver().contains_variable(mVariable)) {
      log("Variable initial value set after initialization:", mVariable, initial);
    } else {
      mVariable.change_value(initial);
      Q_EMIT valueChanged(initial);
    }
  });
}

double Variable::value() const
{
  return mVariable.value();
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
