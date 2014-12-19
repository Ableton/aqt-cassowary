// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#include <ableton/cassowary/Targeted.hpp>

namespace ableton {
namespace cassowary {

Targeted::Targeted(QQuickItem* pParent,
                           Strength::Types type,
                           double weight)
  : ConstraintBase(pParent, type, weight)
{
  auto notify = [this] {
    Q_EMIT actualTargetChanged(actualTarget());
  };
  connect(this, &QQuickItem::parentChanged, notify);
  connect(this, &Targeted::targetChanged, notify);
}

Variable* Targeted::actualTarget()
{
  return mTarget
    ? static_cast<Variable*>(mTarget)
    : dynamic_cast<Variable*>(parentItem());
}

} // namespace cassowary
} // namespace ableton
