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

#pragma once

#include <ableton/cassowary/Contextual.hpp>
#include <ableton/build_system/Warnings.hpp>

namespace ableton {
namespace cassowary {

class Solver : public Contextual
{
  Q_OBJECT

public:
  Solver(QQuickItem* pParent = nullptr);
  ~Solver() override;

  Q_PROPERTY(bool debug MEMBER mDebug NOTIFY debugChanged)
  Q_SIGNAL void debugChanged(bool debug);

protected:
  void componentComplete() override;
  std::shared_ptr<Context> provided() override;
  void addIn(Context&) override {}
  void removeIn(Context&) override {}

private:
  bool mDebug = false;
  std::shared_ptr<Context> mProvided;
};

} // namespace cassowary
} // namespace ableton
