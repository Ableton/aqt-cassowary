// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#include <ableton/cassowary/Context.hpp>

namespace ableton {
namespace cassowary {

Context::Context()
{
  mSolver.set_autosolve(false);
  mSolver.on_variable_change =
    [this](const rhea::variable& variable, rhea::simplex_solver& solver) {
      auto it = mVariableCallbacks.find(variable);
      if (it != mVariableCallbacks.end()) {
        it->second(variable, solver);
      }
    };

  mTimer.setSingleShot(true);
  QObject::connect(&mTimer, &QTimer::timeout, [this] {
    commit();
  });
}

void Context::defer(DeferredCallback fn)
{
  mDeferred.emplace(std::move(fn));
  schedule();
}

void Context::schedule()
{
  if (!mDeferred.empty() && !mTimer.isActive()) {
    mTimer.start();
  }
}

void Context::commit()
{
  auto resolve = !mDeferred.empty();

  while (!mDeferred.empty()) {
    mDeferred.front()();
    mDeferred.pop();
  }

  if (resolve) {
    mSolver.solve();
  }
}

void Context::setVariableCallback(
  rhea::variable variable,
  VariableChangeCallback callback)
{
  if (callback) {
    auto success = mVariableCallbacks.emplace(
      std::move(variable),
      std::move(callback)).second;
    if (!success) {
      throw ContextError { "callback already set for variable" };
    }
  } else {
    mVariableCallbacks.erase(variable);
  }
}


} // namespace cassowary
} // namespace ableton
