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
#include <QtCore/QTimer>
#include <QtCore/QDebug>
#include <rhea/simplex_solver.hpp>
#include <rhea/iostream.hpp>
#include <boost/lexical_cast.hpp>
#include <queue>
ABL_RESTORE_WARNINGS

namespace aqt {
namespace cassowary {

struct ContextError : std::runtime_error
{
  using std::runtime_error::runtime_error;
};

class Context : public std::enable_shared_from_this<Context>
{
public:
  using DeferredCallback = std::function<void()>;

  bool debug = false;

  Context();

  rhea::simplex_solver& solver() { return mSolver; }

  void defer(DeferredCallback fn);
  void commit();
  void resolve();

  template <typename ...Args>
  void log(Args&&... args)
  {
    if (debug) {
      logImpl(qDebug(), "[Aqt.Cassowary]", std::forward<Args>(args)...);
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


  bool mCommiting = false;
  std::queue<DeferredCallback> mDeferred;
  QTimer mTimer;
  rhea::simplex_solver mSolver;
};

} // namespace cassowary
} // namespace aqt
