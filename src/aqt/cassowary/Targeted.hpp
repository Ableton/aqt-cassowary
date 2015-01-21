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

#include <aqt/cassowary/Variable.hpp>
#include <aqt/cassowary/ConstraintBase.hpp>

namespace aqt {
namespace cassowary {

class Targeted : public ConstraintBase
{
  Q_OBJECT

public:
  Targeted(QQuickItem* pParent = nullptr,
           Strength::Types strength = Strength::Required,
           double weight = 1.0);

  Q_PROPERTY(aqt::cassowary::Variable* target
             MEMBER mTarget NOTIFY targetChanged)
  Q_SIGNAL void targetChanged(aqt::cassowary::Variable* target);

  Variable* actualTarget();
  Q_SIGNAL void actualTargetChanged(aqt::cassowary::Variable* target);

private:
  QPointer<aqt::cassowary::Variable> mTarget;
};

} // namespace cassowary
} // namespace aqt
