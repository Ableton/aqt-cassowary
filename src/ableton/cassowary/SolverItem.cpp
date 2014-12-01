// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#include <ableton/cassowary/SolverItem.hpp>

namespace ableton {
namespace cassowary {

SolverItem::SolverItem(QQuickItem* pParent)
  : QQuickItem(pParent)
{
}

Solver* SolverItem::solver() const
{
  return mSolver;
}

void SolverItem::setSolver(Solver* solver)
{
  remove();
  mSolver = solver;
  add();
}

void SolverItem::add()
{
  if (mSolver) {
    addIn(mSolver->solverImpl());
  }
}

void SolverItem::remove()
{
  if (mSolver) {
    removeIn(mSolver->solverImpl());
  }
}

} // namespace cassowary
} // namespace ableton
