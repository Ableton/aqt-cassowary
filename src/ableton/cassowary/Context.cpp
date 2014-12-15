// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#include <ableton/cassowary/Context.hpp>

namespace ableton {
namespace cassowary {

Context::Context()
{
  mSolver.on_variable_change =
    [this](const rhea::variable& variable, rhea::simplex_solver& solver) {
      auto it = mVariableCallbacks.find(variable);
      if (it != mVariableCallbacks.end()) {
        it->second(variable, solver);
      }
    };
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
