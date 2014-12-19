// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#pragma once

#include <ableton/cassowary/Contextual.hpp>
#include <ableton/cassowary/Strength.hpp>

namespace ableton {
namespace cassowary {

class ConstraintBase : public Contextual
{
  Q_OBJECT

public:
  ConstraintBase(QQuickItem* pParent = 0,
                 Strength::Types strength = Strength::Required,
                 double weight = 1.0);

  Q_PROPERTY(ableton::cassowary::Strength::Types
             strength MEMBER mStrength NOTIFY strengthChanged)
  Q_SIGNAL void strengthChanged(ableton::cassowary::Strength::Types strength);

  Q_PROPERTY(double weight MEMBER mWeight NOTIFY weightChanged)
  Q_SIGNAL void weightChanged(double weight);

  Q_PROPERTY(bool when MEMBER mWhen NOTIFY whenChanged)
  Q_SIGNAL void whenChanged(bool when);

protected:
  bool when() const { return mActualWhen; }
  void set(std::shared_ptr<rhea::abstract_constraint> constraint);
  void addIn(Context& ctx) override;
  void removeIn(Context& ctx) override;

private:
  Strength::Types mStrength;
  double mWeight;
  bool mWhen = true;
  bool mActualWhen = true;
  rhea::constraint mConstraint;
};

} // namespace cassowary
} // namespace ableton
