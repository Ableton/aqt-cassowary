// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#pragma once

#include <ableton/cassowary/ConstraintItem.hpp>
#include <ableton/cassowary/Variable.hpp>
#include <limits>

namespace ableton {
namespace cassowary {

class Edit : public ConstraintItem
{
  Q_OBJECT

public:
  Edit(QQuickItem* pParent=0);

  Q_PROPERTY(ableton::cassowary::Variable* target
             MEMBER mTarget NOTIFY targetChanged)
  Q_SIGNAL void targetChanged(ableton::cassowary::Variable* target);

  Q_PROPERTY(double suggested
             MEMBER mSuggested NOTIFY suggestedChanged)
  Q_SIGNAL void suggestedChanged(double suggested);


  Q_INVOKABLE void suggest(double value);

protected:
  void addIn(Context& solver) override;

private:
  QPointer<ableton::cassowary::Variable> mTarget;
  double mSuggested = std::numeric_limits<double>::quiet_NaN();
};

} // namespace cassowary
} // namespace ableton
