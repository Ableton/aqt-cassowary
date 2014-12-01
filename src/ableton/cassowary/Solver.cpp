// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#include <ableton/cassowary/Solver.hpp>

namespace ableton {
namespace cassowary {

Solver::Solver(QQuickItem* pParent)
  : QQuickItem(pParent)
{
}

rhea::simplex_solver& Solver::solverImpl()
{
  return mSolverImpl;
}

} // namespace cassowary
} // namespace ableton
