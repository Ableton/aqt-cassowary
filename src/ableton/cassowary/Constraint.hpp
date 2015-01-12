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

#include <ableton/cassowary/ConstraintBase.hpp>

namespace ableton {
namespace cassowary {

class Constraint : public ConstraintBase
{
  Q_OBJECT

public:
  Constraint(QQuickItem* pParent = nullptr);

  Q_PROPERTY(QVariant expr MEMBER mExpr NOTIFY exprChanged)
  Q_SIGNAL void exprChanged(QVariant expr);

  Q_INVOKABLE QVariant plus(QVariant a, QVariant b);
  Q_INVOKABLE QVariant minus(QVariant a, QVariant b);
  Q_INVOKABLE QVariant times(QVariant a, QVariant b);
  Q_INVOKABLE QVariant divide(QVariant a, QVariant b);
  Q_INVOKABLE QVariant eq(QVariant a, QVariant b);
  Q_INVOKABLE QVariant geq(QVariant a, QVariant b);
  Q_INVOKABLE QVariant leq(QVariant a, QVariant b);

private:
  struct Setter;
  QVariant mExpr;
};

} // namespace cassowary
} // namespace ableton
