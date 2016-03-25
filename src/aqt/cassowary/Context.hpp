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

#pragma once

#include <ableton/build_system/Warnings.hpp>

ABL_DISABLE_WARNINGS
#include <QtCore/QLoggingCategory>
#include <QtCore/QTimer>
#include <QtCore/QDebug>
#include <QtCore/QPointer>
#include <QtQuick/QQuickItem>
#include <rhea/simplex_solver.hpp>
#include <rhea/iostream.hpp>
#include <boost/lexical_cast.hpp>
#include <queue>
ABL_RESTORE_WARNINGS

namespace aqt {
namespace cassowary {

Q_DECLARE_LOGGING_CATEGORY(Logging)

struct ContextError : std::runtime_error
{
  using std::runtime_error::runtime_error;
};

class Context : public std::enable_shared_from_this<Context>
{
public:
  using Id = std::size_t;
  using Callback = std::function<void()>;

  bool debug = false;

  Context(Callback cb);

  rhea::simplex_solver& solver() { return mSolver; }

  void add(rhea::constraint c);
  void remove(rhea::constraint c);
  void suggest(rhea::constraint e, double x);
  void edit(rhea::variable v, double x);
  void notify(rhea::variable v, Callback cb);
  void defer(Callback cb);
  void commit();
  void requestSolve();

  template <typename ...Args>
  void log(Args&& ...args) const
  {
    if (debug) {
      qCDebug(Logging,)
        << format(std::ostringstream{}, std::forward<Args>(args)...).c_str();
    }
  }

private:
  std::string format(std::ostringstream&& s) const
  {
    return s.str();
  }

  template <typename T>
  std::string format(std::ostringstream&& s, T&& x) const
  {
    s << x;
    return format(std::move(s));
  }

  template <typename T, typename ...Ts>
  std::string format(std::ostringstream&& s, T&& x, Ts&&... xs) const
  {
    s << x << " ";
    return format(std::move(s), std::forward<Ts>(xs)...);
  }

  void schedule();

  Callback mSchedule;
  bool mCommiting = false;
  bool mNeedsSolve = false;
  std::unordered_set<rhea::constraint> mAdditions;
  std::unordered_set<rhea::constraint> mRemovals;
  std::unordered_map<rhea::constraint, double> mSuggestions;
  std::vector<std::pair<rhea::variable, double> > mEdits;
  std::vector<Callback> mDeferred;
  std::unordered_map<rhea::variable, Callback> mNotifications;
  rhea::simplex_solver mSolver;
};

template <typename Fn>
struct RheaGuard {
  Fn fn;

  template <typename ...Args>
  auto operator()(Args&&... args)
    -> decltype(fn(std::forward<Args>(args)...))
  {
    try {
      return fn(std::forward<Args>(args)...);
    } catch (rhea::error& err) {
      qCWarning(Logging,)
        << "RHEA ERROR:"
        << err.what();
    }
  }
};

template <typename Fn>
auto rheaGuard(Fn&& fn)
  -> RheaGuard<typename std::decay<Fn>::type>
{
  return { fn };
}

struct ValueTag {};
struct WeightTag {};
struct StrengthTag {};

inline std::ostream& operator<<(std::ostream& os, ValueTag) { return os << "value"; }
inline std::ostream& operator<<(std::ostream& os, WeightTag) { return os << "weight"; }
inline std::ostream& operator<<(std::ostream& os, StrengthTag) { return os << "strength"; }

inline bool contains(rhea::simplex_solver& s, const rhea::variable& v)
{ return s.contains_variable(v); }
inline bool contains(rhea::simplex_solver& s, const rhea::constraint& c)
{ return s.contains_constraint(c); }
inline void change(rhea::variable& v, ValueTag, double x)
{ v.change_value(x); }
inline void change(rhea::constraint& c, StrengthTag, rhea::strength x)
{ c.change_strength(x); }
inline void change(rhea::constraint& c, WeightTag, double x)
{ c.change_weight(x); }
inline void change(rhea::simplex_solver& s, rhea::variable& v, ValueTag, double x)
{ s.suggest(v, x); }
inline void change(rhea::simplex_solver& s, rhea::constraint& c, StrengthTag, rhea::strength x)
{ s.change_strength(c, x); }
inline void change(rhea::simplex_solver& s, rhea::constraint& c, WeightTag, double x)
{ s.change_weight(c, x); }

template <typename EntityT, typename ParameterTagT, typename ValueT>
void change(std::shared_ptr<Context> ctx, EntityT& e, ParameterTagT t, ValueT&& x)
{
  rheaGuard([&] {
    if (ctx) ctx->log("Changing", e, "[", t, "] =", x);
    if (!e.is_nil()) {
      auto contained = ctx && contains(ctx->solver(), e);
      if (contained) {
        change(ctx->solver(), e, t, std::forward<ValueT>(x));
        ctx->requestSolve();
      }
      else {
        change(e, t, std::forward<ValueT>(x));
      }
    }
  })();
}

} // namespace cassowary
} // namespace aqt
