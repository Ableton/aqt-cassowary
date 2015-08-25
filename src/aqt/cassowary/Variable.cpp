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

#include <aqt/cassowary/Variable.hpp>

namespace aqt {
namespace cassowary {

Variable::Variable(QQuickItem* pParent)
  : Contextual(pParent)
  , mVariable([this] (double value) {
    if (!rhea::near_zero(value - mLastValue)) {
      log("    ... updated: ", mVariable, value);
      mLastValue = value;
      auto ctx = context();
      auto notification = [this, value] {
        log("    ... notify: ", mVariable, value);
        Q_EMIT valueChanged(value);
      };
      if (ctx) {
        ctx->notify(mVariable, guarded(notification));
      } else {
        notification();
      }
    }
  })
{
  connect(this, &Variable::initialChanged, [this] (double initial) {
    auto ctx = context();
    if (!std::isnan(initial)) {
      if (ctx && ctx->solver().contains_variable(mVariable)) {
        log("Initial value set after initialization for", mVariable, "=", initial);
      } else {
        log("Setting initial value for ", mVariable, "=", initial);
        mVariable.change_value(initial);
      }
    }
  });
}

void Variable::addIn(Context&)
{
  setValue(mInitial);
}

double Variable::value() const
{
  return mVariable.value();
}

void Variable::setValue(double value)
{
  auto ctx = context();
  if (ctx) ctx->edit(mVariable, value);
}

const rhea::variable& Variable::variableImpl() const
{
  return mVariable;
}

} // namespace cassowary
} // namespace aqt
