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

Q_LOGGING_CATEGORY(Logging, "aqt.cassowary")

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

void Context::add(rhea::constraint c)
{
  if (!c.is_nil() && !mRemovals.erase(c)) {
    mAdditions.emplace(std::move(c));
    schedule();
  }
}

void Context::remove(rhea::constraint c)
{
  if (!c.is_nil() && !mAdditions.erase(c)) {
    mRemovals.emplace(std::move(c));
    schedule();
  }
}

void Context::suggest(rhea::variable v, double x)
{
  mSuggestions[v] = x;
  schedule();
}

void Context::requestSolve()
{
  mNeedsSolve = true;
  schedule();
}

void Context::schedule()
{
  if (mSchedule) {
    mSchedule();
  }
}

namespace {

template <typename T>
T swapD(T& orig)
{
  using std::swap;

  auto def = T{};
  swap(orig, def);
  return def;
}

template <typename ConstraintsT>
bool commitConstraints(
  Context& ctx,
  const ConstraintsT& remove, const ConstraintsT& add, bool solve)
{
  std::for_each(
    remove.begin(), remove.end(),
    rheaGuard([&] (const rhea::constraint& c) {
      ctx.log("  Remove:", c);
      ctx.solver().remove_constraint(c);
    }));

  std::for_each(
    add.begin(), add.end(),
    rheaGuard([&] (const rhea::constraint& c) {
      ctx.log("  Add:", c);
      ctx.solver().add_constraint(c);
    }));

  if (solve || !remove.empty() || !add.empty()) {
    rheaGuard([&] {
      ctx.log(" Solving");
      ctx.solver().solve();
    })();
    return true;
  }
  return false;
}

template <typename SuggestionsT>
bool commitSuggestions(
  Context& ctx,
  const SuggestionsT& suggestions)
{
  std::for_each(
    suggestions.begin(), suggestions.end(),
    rheaGuard([&] (const std::pair<rhea::variable, double>& s) {
      ctx.log("  Suggesting:", s.first, "=", s.second);
      ctx.solver().suggest_value(s.first, s.second);
    }));

  if (!suggestions.empty()) {
    rheaGuard([&] {
      ctx.log(" Resolving");
      ctx.solver().resolve();
    })();
    return true;
  }
  return false;
}

template <typename DeferredT>
bool commitDeferred(Context&, const DeferredT& deferred)
{
  for (auto& d : deferred) d();
  return !deferred.empty();
}

} // anonymous namespace

void Context::commit()
{
  if (!mCommiting) {
    auto _ = shared_from_this();
    auto __ = guard([this] { mCommiting = false; });
    mCommiting = true;

    log("Commiting... ");
    while(commitConstraints(
            *this, swapD(mRemovals), swapD(mAdditions), swapD(mNeedsSolve))
       || commitSuggestions(
         *this, swapD(mSuggestions))
       || commitDeferred(
         *this, swapD(mDeferred)));
    log("...commit finished");
  }
}

} // namespace cassowary
} // namespace aqt
