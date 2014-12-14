// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#include <ableton/cassowary/Solver.hpp>

namespace ableton {
namespace cassowary {

Solver::Solver(QQuickItem* pParent)
  : SolverBase(pParent)
  , mSolverImpl(std::make_shared<rhea::simplex_solver>())
{
}

Solver::~Solver()
{
}

std::shared_ptr<rhea::simplex_solver> Solver::solverImpl()
{
  return mSolverImpl;
}

} // namespace cassowary
} // namespace ableton
