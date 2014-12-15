// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#include <ableton/cassowary/ConstraintItem.hpp>

namespace ableton {
namespace cassowary {

ConstraintItem::ConstraintItem(QQuickItem* pParent,
                               Strength::Types strength,
                               double weight)
  : SolverItem(pParent)
  , mStrength(strength)
  , mWeight(weight)
{
  connect(this, &ConstraintItem::strengthChanged, [this](Strength::Types) {
    remove();
    if (!mConstraint.is_nil())
      mConstraint.change_strength(Strength::impl(mStrength));
    add();
  });

  connect(this, &ConstraintItem::weightChanged, [this](double) {
    remove();
    if (!mConstraint.is_nil())
      mConstraint.change_weight(mWeight);
    add();
  });

  connect(this, &ConstraintItem::whenChanged, [this](bool when) {
    if (when) {
      add();
    } else {
      remove();
    }
  });
}

void ConstraintItem::set(std::shared_ptr<rhea::abstract_constraint> constraint)
{
  remove();
  mConstraint = std::move(constraint);
  mConstraint.change_weight(mWeight);
  mConstraint.change_strength(Strength::impl(mStrength));
  add();
}

void ConstraintItem::addIn(Context& ctx)
{
  if (mWhen && !mConstraint.is_nil()) {
    ctx.solver().add_constraint(mConstraint);
  }
}

void ConstraintItem::removeIn(Context& ctx)
{
  if (!mConstraint.is_nil()) {
    ctx.solver().remove_constraint(mConstraint);
  }
}

} // namespace cassowary
} // namespace ableton
