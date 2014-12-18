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
    defer([this] {
      if (!mConstraint.is_nil()) {
        mConstraint.change_strength(Strength::impl(mStrength));
      }
    });
    add();
  });

  connect(this, &ConstraintItem::weightChanged, [this](double) {
    remove();
    defer([this] {
      if (!mConstraint.is_nil()) {
        mConstraint.change_weight(mWeight);
      }
    });
    add();
  });

  connect(this, &ConstraintItem::whenChanged, [this](bool when) {
    remove();
    defer ([this, when] {
      mActualWhen = when;
    });
    add();
  });
}

void ConstraintItem::set(std::shared_ptr<rhea::abstract_constraint> constraint)
{
  remove();
  defer([this, constraint]() mutable {
    mConstraint = std::move(constraint);
    if (!mConstraint.is_nil()) {
      mConstraint.change_weight(mWeight);
      mConstraint.change_strength(Strength::impl(mStrength));
    }
  });
  add();
}

void ConstraintItem::addIn(Context& ctx)
{
  if (when() && !mConstraint.is_nil()) {
    log("Add:", mConstraint);
    try {
      ctx.solver().add_constraint(mConstraint);
    } catch (const rhea::required_failure&) {
      log("Required constraint can't be satisfied:", mConstraint);
    }
  }
}

void ConstraintItem::removeIn(Context& ctx)
{
  if (!mConstraint.is_nil()) {
    try {
      log("Remove:", mConstraint);
      ctx.solver().remove_constraint(mConstraint);
    } catch (const rhea::constraint_not_found&) {
      log("Constraint not found:", mConstraint);
    }
  }
}

} // namespace cassowary
} // namespace ableton
