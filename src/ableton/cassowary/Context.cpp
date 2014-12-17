// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#include <ableton/cassowary/Context.hpp>

namespace ableton {
namespace cassowary {

Context::Context()
{
  mSolver.set_autosolve(false);
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
    mSolver.resolve();
  }
}

} // namespace cassowary
} // namespace ableton
