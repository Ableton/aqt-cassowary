// Copyright: 2014, Ableton AG, Berlin, all rights reserved
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy,
// modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
// BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <aqt/cassowary/Context.hpp>

namespace aqt {
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
} // namespace aqt
