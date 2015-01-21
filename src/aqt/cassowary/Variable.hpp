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

#include <aqt/cassowary/Contextual.hpp>
#include <ableton/build_system/Warnings.hpp>

ABL_DISABLE_WARNINGS
#include <rhea/variable.hpp>
ABL_RESTORE_WARNINGS

namespace aqt {
namespace cassowary {

class Variable : public Contextual
{
  Q_OBJECT

public:
  Variable(QQuickItem* pParent = nullptr);

  Q_PROPERTY(double value READ value NOTIFY valueChanged)
  double value() const;
  Q_SIGNAL void valueChanged(double value);

  Q_PROPERTY(double initial MEMBER mInitial NOTIFY initialChanged)
  Q_SIGNAL void initialChanged(double initial);

  const rhea::variable& variableImpl() const;

protected:
  void addIn(Context&) override {}
  void removeIn(Context&) override {}

private:
  rhea::variable mVariable;
  double mInitial = 0.0;
};

} // namespace cassowary
} // namespace aqt
