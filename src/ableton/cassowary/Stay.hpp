// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#pragma once

#include <ableton/cassowary/ConstraintItem.hpp>
#include <ableton/cassowary/Variable.hpp>

namespace ableton {
namespace cassowary {

class Stay : public ConstraintItem
{
  Q_OBJECT

public:
  Stay(QQuickItem* pParent=0);

  Q_PROPERTY(ableton::cassowary::Variable* target
             MEMBER mTarget NOTIFY targetChanged)
  Q_SIGNAL void targetChanged(ableton::cassowary::Variable* target);

private:
  QPointer<ableton::cassowary::Variable> mTarget;
};

} // namespace cassowary
} // namespace ableton
