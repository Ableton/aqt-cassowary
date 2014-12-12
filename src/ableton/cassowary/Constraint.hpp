// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#pragma once

#include <ableton/cassowary/SolverItem.hpp>
#include <ableton/cassowary/Strength.hpp>

namespace ableton {
namespace cassowary {

class Constraint : public SolverItem
{
  Q_OBJECT

public:
  Constraint(QQuickItem* pParent = 0);

  Q_PROPERTY(QVariant expr MEMBER mExpr NOTIFY exprChanged)
  Q_SIGNAL void exprChanged(QVariant expr);

  Q_PROPERTY(ableton::cassowary::Strength::Types strength MEMBER mStrength NOTIFY strengthChanged)
  Q_SIGNAL void strengthChanged(ableton::cassowary::Strength::Types strength);

  Q_PROPERTY(double weight MEMBER mWeight NOTIFY weightChanged)
  Q_SIGNAL void weightChanged(double weight);

  Q_INVOKABLE QVariant plus(QVariant a, QVariant b);
  Q_INVOKABLE QVariant minus(QVariant a, QVariant b);
  Q_INVOKABLE QVariant times(QVariant a, QVariant b);
  Q_INVOKABLE QVariant divide(QVariant a, QVariant b);
  Q_INVOKABLE QVariant eq(QVariant a, QVariant b);
  Q_INVOKABLE QVariant geq(QVariant a, QVariant b);
  Q_INVOKABLE QVariant leq(QVariant a, QVariant b);

private:
  void addIn(rhea::simplex_solver& solver) override;
  void removeIn(rhea::simplex_solver& solver) override;

  QVariant mExpr;
  Strength::Types mStrength = Strength::Required;
  double mWeight = 1;
  rhea::constraint mConstraint;
};

} // namespace cassowary
} // namespace ableton
