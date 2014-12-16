// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#pragma once

#include <ableton/build_system/Warnings.hpp>

ABL_DISABLE_WARNINGS
#include <QtCore/QTimer>
#include <QtCore/QDebug>
#include <rhea/simplex_solver.hpp>
#include <rhea/iostream.hpp>
#include <boost/lexical_cast.hpp>
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
  using DeferredCallback = std::function<void()>;

  bool debug = false;

  Context();

  rhea::simplex_solver& solver() { return mSolver; }

  void defer(DeferredCallback fn);
  void commit();

  template <typename ...Args>
  void log(Args&&... args)
  {
    if (debug) {
      logImpl(qDebug(), "[Ableton.Cassowary]", std::forward<Args>(args)...);
    }
  }

private:
  void schedule();

  void logImpl(QDebug&&) {}
  template <typename Arg, typename ...Args>
  void logImpl(QDebug&& out, Arg&& arg, Args&&... args)
  {
    out << boost::lexical_cast<std::string>(std::forward<Arg>(arg)).c_str();
    logImpl(std::move(out), std::forward<Args>(args)...);
  }

  std::queue<DeferredCallback> mDeferred;
  QTimer mTimer;
  rhea::simplex_solver mSolver;
};

} // namespace cassowary
} // namespace ableton
