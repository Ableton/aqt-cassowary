// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#pragma once

#include <ableton/cassowary/TargetedItem.hpp>
#include <ableton/cassowary/Variable.hpp>

namespace ableton {
namespace cassowary {

class Stay : public TargetedItem
{
  Q_OBJECT

public:
  Stay(QQuickItem* pParent=0);
};

} // namespace cassowary
} // namespace ableton
