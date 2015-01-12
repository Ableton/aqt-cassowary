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
#include <ableton/cassowary/Strength.hpp>

namespace ableton {
namespace cassowary {

class ConstraintBase : public Contextual
{
  Q_OBJECT

public:
  ConstraintBase(QQuickItem* pParent = nullptr,
                 Strength::Types strength = Strength::Required,
                 double weight = 1.0);

  Q_PROPERTY(ableton::cassowary::Strength::Types
             strength MEMBER mStrength NOTIFY strengthChanged)
  Q_SIGNAL void strengthChanged(ableton::cassowary::Strength::Types strength);

  Q_PROPERTY(double weight MEMBER mWeight NOTIFY weightChanged)
  Q_SIGNAL void weightChanged(double weight);

  Q_PROPERTY(bool when MEMBER mWhen NOTIFY whenChanged)
  Q_SIGNAL void whenChanged(bool when);

protected:
  bool when() const { return mActualWhen; }
  void set(std::shared_ptr<rhea::abstract_constraint> constraint);
  void addIn(Context& ctx) override;
  void removeIn(Context& ctx) override;

private:
  Strength::Types mStrength;
  double mWeight;
  bool mWhen = true;
  bool mActualWhen = true;
  rhea::constraint mConstraint;
};

} // namespace cassowary
} // namespace ableton
