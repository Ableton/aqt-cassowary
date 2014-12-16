// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#pragma once

#include <ableton/build_system/Warnings.hpp>

ABL_DISABLE_WARNINGS
#include <QtCore/QTimer>
#include <QtCore/QDebug>
#include <rhea/simplex_solver.hpp>
#include <queue>
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
  using DeferredCallback = std::function<void()>;

  Context();

  rhea::simplex_solver& solver() { return mSolver; }

  void defer(DeferredCallback fn);
  void commit();

  void setVariableCallback(
    rhea::variable variable,
    VariableChangeCallback callback);

private:
  void schedule();

  std::queue<DeferredCallback> mDeferred;
  QTimer mTimer;
  std::unordered_map<rhea::variable, VariableChangeCallback> mVariableCallbacks;
  rhea::simplex_solver mSolver;
};

} // namespace cassowary
} // namespace ableton
