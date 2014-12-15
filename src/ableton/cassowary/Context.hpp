// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#pragma once

#include <ableton/build_system/Warnings.hpp>

ABL_DISABLE_WARNINGS
#include <rhea/simplex_solver.hpp>
ABL_RESTORE_WARNINGS

namespace ableton {
namespace cassowary {

struct ContextError : std::runtime_error
{
  using std::runtime_error::runtime_error;
};

class Context
{
public:
  using VariableChangeCallback = rhea::simplex_solver::variable_cb;

  Context();

  rhea::simplex_solver& solver() { return mSolver; }

  void setVariableCallback(
    rhea::variable variable,
    VariableChangeCallback callback);

private:
  std::unordered_map<rhea::variable, VariableChangeCallback> mVariableCallbacks;
  rhea::simplex_solver mSolver;
};

} // namespace cassowary
} // namespace ableton
