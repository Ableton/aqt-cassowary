// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#include <ableton/cassowary/Context.hpp>

namespace ableton {
namespace cassowary {

namespace {

template <typename Fn>
struct Guard {
  Fn cleanup;
  Guard(Fn c) : cleanup(std::move(c)) {}
  Guard(Guard&& g) : cleanup(std::move(g.cleanup)) {}
  Guard(const Guard&) = delete;
  ~Guard() { cleanup(); }
};

template <typename Fn>
auto guard(Fn&& fn) -> Guard<typename std::decay<Fn>::type>
{
  return Guard<typename std::decay<Fn>::type>{ std::forward<Fn>(fn) };
}

} // anonymous namespace

Context::Context()
{
  mSolver.set_autosolve(false);
  mTimer.setSingleShot(true);
  QObject::connect(&mTimer, &QTimer::timeout, &mTimer, [this] {
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
  if (!mCommiting && !mDeferred.empty() && !mTimer.isActive()) {
    mTimer.start();
  }
}

void Context::resolve()
{
  mSolver.solve();
  mSolver.resolve();
}

void Context::commit()
{
  if (!mCommiting) {
    auto c = shared_from_this();
    auto g = guard([this] { mCommiting = false; });
    auto doStuff = !mDeferred.empty();
    mCommiting = true;

    log("Commiting...", mDeferred.size());
    while (!mDeferred.empty()) {
      mDeferred.front()();
      mDeferred.pop();
    }

    if (doStuff) {
      resolve();
    }
    log("...commit finished");
  }
}

} // namespace cassowary
} // namespace ableton
