// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#pragma once

#include <ableton/cassowary/SolverItem.hpp>

namespace ableton {
namespace cassowary {

class SolverProxy : public SolverItem
{
  Q_OBJECT

public:
  SolverProxy(QQuickItem* pParent = 0);
  void addIn(Context&) override {}
  void removeIn(Context&) override {}
};

} // namespace cassowary
} // namespace ableton
