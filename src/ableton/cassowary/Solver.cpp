// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#include <ableton/cassowary/Solver.hpp>

namespace ableton {
namespace cassowary {

Solver::Solver(QQuickItem* pParent)
  : Contextual(pParent)
{
  connect(this, &Solver::debugChanged, [this] (bool debug) {
    if (mProvided) {
      mProvided->debug = debug;
    }
  });
}

Solver::~Solver()
{
}

void Solver::componentComplete()
{
  Contextual::componentComplete();
  if (!provider()) {
    mProvided = std::make_shared<Context>();
    mProvided->debug = mDebug;
    updateContext();
  }
}

std::shared_ptr<Context> Solver::provided()
{
  return mProvided;
}


} // namespace cassowary
} // namespace ableton
