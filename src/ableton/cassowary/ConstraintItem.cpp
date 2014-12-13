// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#include <ableton/cassowary/ConstraintItem.hpp>

namespace ableton {
namespace cassowary {

ConstraintItem::ConstraintItem(QQuickItem* pParent)
  : SolverItem(pParent)
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
}

void ConstraintItem::set(std::shared_ptr<rhea::abstract_constraint> constraint)
{
  remove();
  mConstraint = std::move(constraint);
  mConstraint.change_weight(mWeight);
  mConstraint.change_strength(Strength::impl(mStrength));
  add();
}

void ConstraintItem::addIn(rhea::simplex_solver& solver)
{
  if (!mConstraint.is_nil()) {
    solver.add_constraint(mConstraint);
  }
}

void ConstraintItem::removeIn(rhea::simplex_solver& solver)
{
  if (!mConstraint.is_nil()) {
    solver.remove_constraint(mConstraint);
  }
}

} // namespace cassowary
} // namespace ableton
