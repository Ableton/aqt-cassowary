// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#pragma once

#include <ableton/cassowary/Contextual.hpp>
#include <ableton/build_system/Warnings.hpp>

namespace ableton {
namespace cassowary {

class Solver : public Contextual
{
  Q_OBJECT

public:
  Solver(QQuickItem* pParent = 0);
  ~Solver() override;

  Q_PROPERTY(bool debug MEMBER mDebug NOTIFY debugChanged)
  Q_SIGNAL void debugChanged(bool debug);

protected:
  void componentComplete() override;
  std::shared_ptr<Context> provided() override;
  void addIn(Context&) override {}
  void removeIn(Context&) override {}

private:
  bool mDebug = false;
  std::shared_ptr<Context> mProvided;
};

} // namespace cassowary
} // namespace ableton
