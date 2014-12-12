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
  Q_EMIT solverChanged(solver);
}

rhea::simplex_solver& SolverItem::solverImpl()
{
  if (mSolver)
    return mSolver->solverImpl();
  throw SolverItemError { "No solver available" };
}

const rhea::simplex_solver& SolverItem::solverImpl() const
{
  if (mSolver)
    return mSolver->solverImpl();
  throw SolverItemError { "No solver available" };
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
