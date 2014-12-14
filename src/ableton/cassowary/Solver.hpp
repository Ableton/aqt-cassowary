// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#pragma once

#include <ableton/cassowary/SolverBase.hpp>

namespace ableton {
namespace cassowary {

class Solver : public SolverBase
{
  Q_OBJECT

public:
  Solver(QQuickItem* pParent = 0);
  ~Solver() override;
  std::shared_ptr<rhea::simplex_solver> solverImpl() override;

private:
  std::shared_ptr<rhea::simplex_solver> mSolverImpl;
};

} // namespace cassowary
} // namespace ableton
