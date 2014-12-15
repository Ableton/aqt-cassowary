// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#pragma once

#include <ableton/build_system/Warnings.hpp>

ABL_DISABLE_WARNINGS
#include <rhea/simplex_solver.hpp>
ABL_RESTORE_WARNINGS

namespace ableton {
namespace cassowary {

class Context
{
public:
  rhea::simplex_solver& solver() { return mSolver; }

private:
  rhea::simplex_solver mSolver;
};

} // namespace cassowary
} // namespace ableton
