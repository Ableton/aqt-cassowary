// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#include <ableton/cassowary/Variable.hpp>

namespace ableton {
namespace cassowary {

Variable::Variable(QQuickItem* pParent)
  : QQuickItem(pParent)
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

rhea::linear_expression Variable::expressionImpl() const
{
  return rhea::linear_expression(mVariable);
}

} // namespace cassowary
} // namespace ableton
