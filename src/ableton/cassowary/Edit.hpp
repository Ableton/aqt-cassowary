// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#pragma once

#include <ableton/cassowary/ConstraintItem.hpp>
#include <ableton/cassowary/Variable.hpp>

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

  Q_INVOKABLE void suggest(double value);

private:
  QPointer<ableton::cassowary::Variable> mTarget;
};

} // namespace cassowary
} // namespace ableton
