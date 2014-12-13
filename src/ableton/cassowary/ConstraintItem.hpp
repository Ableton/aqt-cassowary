// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#pragma once

#include <ableton/cassowary/SolverItem.hpp>
#include <ableton/cassowary/Strength.hpp>

namespace ableton {
namespace cassowary {

class ConstraintItem : public SolverItem
{
  Q_OBJECT

public:
  ConstraintItem(QQuickItem* pParent = 0,
                 Strength::Types strength = Strength::Required,
                 double weight = 1.0);

  Q_PROPERTY(ableton::cassowary::Strength::Types
             strength MEMBER mStrength NOTIFY strengthChanged)
  Q_SIGNAL void strengthChanged(ableton::cassowary::Strength::Types strength);

  Q_PROPERTY(double weight MEMBER mWeight NOTIFY weightChanged)
  Q_SIGNAL void weightChanged(double weight);

protected:
  void set(std::shared_ptr<rhea::abstract_constraint> constraint);

private:
  void addIn(rhea::simplex_solver& solver) override;
  void removeIn(rhea::simplex_solver& solver) override;

  Strength::Types mStrength;
  double mWeight;
  rhea::constraint mConstraint;
};

} // namespace cassowary
} // namespace ableton
