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
#include <aqt/cassowary/Strength.hpp>
#include <ableton/build_system/Warnings.hpp>

ABL_DISABLE_WARNINGS
#include <rhea/variable.hpp>
ABL_RESTORE_WARNINGS

namespace aqt {
namespace cassowary {

/*!
 *  @ingroup qml
 *  Represents a variable whose value will be determined by a
 *  constraint solver.
 *
 *  @par Example
 *  @code
 *  Solver {
 *    Variable {
 *      id: vX
 *      initial: 100
 *    }
 *  }
 *  Rectangle {
 *    x: vX.value
 *  }
 *  @endcode
 *
 *  @par Import in QML
 *  `import Aqt.Cassowary 1.0`
 */
class Variable : public Contextual
{
  Q_OBJECT

public:
  Variable(QQuickItem* pParent = nullptr);

  /*!
   *  Contains the current value as determined by the constrained
   *  solver.  The value will be updated automatically after the next
   *  solver update, in the presence of changes in the constraints of
   *  the system.
   *
   *  @note Setting the value only *suggests* the setting to the
   *        solver.  Also, the suggestion is not applied inmediatelly,
   *        but in the next solver update.
   *
   *  @todo Add syntax to specify the strength and weight of the
   *        suggestions done via the setter.
   */
  Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged)
  double value() const;
  void setValue(double);
  Q_SIGNAL void valueChanged(double value);

  Q_INVOKABLE void suggest(double value,
                           int /* Strength::Types */ strength,
                           double weight);

  /*!
   *  Sets the initial value of this variable.  If `NaN` is set, the
   *  variable will have an implicit initial value of 0.  However,
   *  explicitly set initial values will are considered *strong*, but
   *  not the implicit ones even if the later variable has supporting
   *  *stay constraints*.
   *
   *  @see Stay
   *
   *  @todo Like for the `value` property setter, it would be nice to
   *        be able to specify the strength and weight of this
   *        suggestion to the solver.
   */
  Q_PROPERTY(double initial MEMBER mInitial NOTIFY initialChanged)
  Q_SIGNAL void initialChanged(double initial);

  const rhea::variable& variableImpl() const;

protected:
  void addIn(Context&) override;
  void removeIn(Context&) override {}

private:
  rhea::variable mVariable;
  double mLastValue = 0;
  double mInitial = std::numeric_limits<double>::quiet_NaN();
};

} // namespace cassowary
} // namespace aqt
