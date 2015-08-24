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

void Context::suggest(rhea::constraint e, double x)
{
  if (!std::isnan(x)) {
    mSuggestions[e] = x;
    schedule();
  }
}

void Context::edit(rhea::variable v, double x)
{
  if (!std::isnan(x)) {
    mEdits.emplace_back(std::move(v), x);
    schedule();
  }
}

void Context::notify(rhea::variable v, Callback cb)
{
  mNotifications[v] = cb;
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

template <typename SuggestionsT, typename EditsT>
bool commitSuggestions(
  Context& ctx,
  const SuggestionsT& suggestions,
  const EditsT& edits)
{
  using edit_t = typename EditsT::value_type;
  using suggestion_t = typename SuggestionsT::value_type;

  if (!suggestions.empty() || !edits.empty()) {
    std::for_each(
      edits.begin(), edits.end(),
      rheaGuard([&] (const edit_t& e) {
        ctx.log("  Add edit var:", e.first);
        ctx.solver().add_edit_var(e.first);
      }));

    if (!edits.empty()) {
      rheaGuard([&] {
        ctx.log("  Reseting stays...");
        ctx.solver().reset_stay_constants();
        ctx.requestSolve();
      })();
    }

    std::for_each(
      suggestions.begin(), suggestions.end(),
      rheaGuard([&] (const suggestion_t& s) {
        ctx.log("  Suggesting:", s.first, "=", s.second);
        try {
          ctx.solver().suggest_value(s.first, s.second);
        } catch (const rhea::edit_misuse&) {
          ctx.log("  Suggestion had no effect");
        }
      }));

    std::for_each(
      edits.begin(), edits.end(),
      rheaGuard([&] (const edit_t& s) {
        ctx.log("  Editing:", s.first, "=", s.second);
        try {
          ctx.solver().suggest_value(s.first, s.second);
        } catch (const rhea::edit_misuse&) {
          ctx.log("  Edit had no effect");
        }
      }));

    rheaGuard([&] {
      ctx.log(" Resolving");
      ctx.solver().resolve();
    })();

    std::for_each(
      edits.begin(), edits.end(),
      rheaGuard([&] (const edit_t& e) {
        ctx.log("  Remove edit var:", e.first);
        ctx.solver().remove_edit_var(e.first);
      }));

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

template <typename NotificationT>
bool commitNotifications(Context&, const NotificationT& notifications)
{
  for (auto& n : notifications) n.second();
  return !notifications.empty();
}

} // anonymous namespace

void Context::commit()
{
  if (!mCommiting) {
    auto _ = shared_from_this();
    auto __ = guard([this] { mCommiting = false; });
    mCommiting = true;

    log("Commiting... ");
    bool notDone = true;
    while (notDone) {
      notDone = false;
      notDone |= commitConstraints(
        *this, swapD(mRemovals), swapD(mAdditions), swapD(mNeedsSolve));
      notDone |= commitSuggestions(
        *this, swapD(mSuggestions), swapD(mEdits));
      notDone |= commitDeferred(
        *this, swapD(mDeferred));
      notDone |= commitNotifications(
        *this, swapD(mNotifications));
    }
    log("...commit finished");
  }
}

} // namespace cassowary
} // namespace aqt
