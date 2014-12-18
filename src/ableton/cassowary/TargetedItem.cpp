// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#include <ableton/cassowary/TargetedItem.hpp>

namespace ableton {
namespace cassowary {

TargetedItem::TargetedItem(QQuickItem* pParent,
                           Strength::Types type,
                           double weight)
  : ConstraintItem(pParent, type, weight)
{
  auto notify = [this] {
    Q_EMIT actualTargetChanged(actualTarget());
  };
  connect(this, &QQuickItem::parentChanged, notify);
  connect(this, &TargetedItem::targetChanged, notify);
}

Variable* TargetedItem::actualTarget()
{
  return mTarget
    ? static_cast<Variable*>(mTarget)
    : dynamic_cast<Variable*>(parentItem());
}

} // namespace cassowary
} // namespace ableton
