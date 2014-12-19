// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#include <ableton/cassowary/ConstraintBase.hpp>

namespace ableton {
namespace cassowary {

ConstraintBase::ConstraintBase(QQuickItem* pParent,
                               Strength::Types strength,
                               double weight)
  : Contextual(pParent)
  , mStrength(strength)
  , mWeight(weight)
{
  connect(this, &ConstraintBase::strengthChanged, [this](Strength::Types) {
    update([this] {
      if (!mConstraint.is_nil()) {
        mConstraint.change_strength(Strength::impl(mStrength));
      }
    });
  });

  connect(this, &ConstraintBase::weightChanged, [this](double) {
    update([this] {
      if (!mConstraint.is_nil()) {
        mConstraint.change_weight(mWeight);
      }
    });
  });

  connect(this, &ConstraintBase::whenChanged, [this](bool when) {
    update ([this, when] {
      mActualWhen = when;
    });
  });
}

void ConstraintBase::set(std::shared_ptr<rhea::abstract_constraint> constraint)
{
  update([this, constraint]() mutable {
    mConstraint = std::move(constraint);
    if (!mConstraint.is_nil()) {
      mConstraint.change_weight(mWeight);
      mConstraint.change_strength(Strength::impl(mStrength));
    }
  });
}

void ConstraintBase::addIn(Context& ctx)
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

void ConstraintBase::removeIn(Context& ctx)
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
