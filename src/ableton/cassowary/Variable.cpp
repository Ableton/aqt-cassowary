// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#include <ableton/cassowary/Variable.hpp>

namespace ableton {
namespace cassowary {

Variable::Variable(QQuickItem* pParent)
  : SolverItem(pParent)
  , mVariable([this] (double value) {
      Q_EMIT valueChanged(value);
    })
{
  connect(this, &Variable::initialChanged, [this] (double initial) {
    auto ctx = context();
    if (ctx && ctx->solver().contains_variable(mVariable)) {
      log("Variable initial value set after initialization:", mVariable, initial);
    } else {
      mVariable.change_value(initial);
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

} // namespace cassowary
} // namespace ableton
