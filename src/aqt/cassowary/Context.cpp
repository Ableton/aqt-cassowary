// Copyright (c) 2014, 2015 Ableton AG, Berlin
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

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

Context::Context(Callback schedule)
  : mSchedule(std::move(schedule))
{
  mSolver.set_autosolve(false);
}

void Context::defer(Callback fn)
{
  mDeferred.emplace_back(std::move(fn));
  schedule();
}

void Context::add(Id id, Callback fn)
{
  if (!mRemovals.erase(id)) {
    mAdditions[id] = std::move(fn);
  }
}

void Context::remove(Id id, Callback fn)
{
  if (!mAdditions.erase(id)) {
    mRemovals[id] = std::move(fn);
  }
}

void Context::schedule()
{
  if (mSchedule && mDeferred.size() == 1) {
    mSchedule();
  }
}

void Context::resolve()
{
  mSolver.solve();
  mSolver.resolve();
}

namespace {

template <typename T>
T swapDefault(T& orig)
{
  T def;
  using std::swap;
  swap(orig, def);
  return def;
}

void call(Context::Callback& fn) { fn(); }
void call(std::pair<const Context::Id, Context::Callback>& p) { p.second(); }

template <typename T>
bool dispatch(T& orig)
{
  auto fns = swapDefault(orig);
  for (auto& fn : fns) {
    call(fn);
  }
}

} // anoynomus namespace

void Context::commit()
{
  if (!mCommiting) {
    auto c = shared_from_this();
    auto g = guard([this] { mCommiting = false; });
    auto dispatched = false;
    mCommiting = true;

    log("Commiting... ");
    while(!mRemovals.empty()  ||
          !mAdditions.empty() ||
          !mDeferred.empty()) {
      log("[-", mRemovals.size(),  "]",
          "[+", mAdditions.size(), "]",
          "[!", mDeferred.size(),  "]");
      dispatched = true;
      dispatch(mRemovals);
      dispatch(mAdditions);
      dispatch(mDeferred);
    }

    if (dispatched) {
      resolve();
    }
    log("...commit finished");
  }
}

} // namespace cassowary
} // namespace aqt
