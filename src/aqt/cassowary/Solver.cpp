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

#include <aqt/cassowary/Solver.hpp>

namespace aqt {
namespace cassowary {

Solver::Solver(QQuickItem* pParent)
  : Contextual(pParent)
{
  connect(this, &Solver::debugChanged, [this] (bool debug) {
    if (mProvided) {
      mProvided->debug = debug;
    }
  });
}

Solver::~Solver()
{
}

void Solver::componentComplete()
{
  Contextual::componentComplete();
  if (!provider()) {
    mProvided = std::make_shared<Context>();
    mProvided->debug = mDebug;
    updateContext();
  }
}

std::shared_ptr<Context> Solver::provided()
{
  return mProvided;
}


} // namespace cassowary
} // namespace aqt
