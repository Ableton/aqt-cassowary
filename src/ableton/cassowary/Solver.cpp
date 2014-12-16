// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#include <ableton/cassowary/Solver.hpp>

namespace ableton {
namespace cassowary {

Solver::Solver(QQuickItem* pParent)
  : Contextual(pParent)
  , mContext(std::make_shared<Context>())
{
}

Solver::~Solver()
{
}

bool Solver::debug() const
{
  return mContext->debug;
}

void Solver::setDebug(bool debug)
{
  mContext->debug = debug;
  Q_EMIT debugChanged(debug);
}

std::shared_ptr<Context> Solver::context()
{
  return mContext;
}

} // namespace cassowary
} // namespace ableton
