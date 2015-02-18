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

#include <aqt/cassowary/Targeted.hpp>
#include <limits>

namespace aqt {
namespace cassowary {

/*!
 *  An edit constraint allows to suggest values for a variable.  It
 *  can be used to represent the user's wish.
 *
 *  @par Example
 *  @code
 *  Solver {
 *      Variable {
 *          id: vX
 *          Stay {}
 *          Edit {
 *              suggested: mouseArea.mouseX
 *              when: mouseArea.pressed
 *          }
 *      }
 *  }
 *  MouseArea { id: mouseArea }
 *  @endcode
 *
 *  @par Import in QML
 *  `import Aqt.Cassowary 1.0`
 *
 *  @since 1.0
 *  @see Stay
 *  @see Targeted
 */
class Edit : public Targeted
{
  Q_OBJECT

public:
  Edit(QQuickItem* pParent=0);

  /*!
   *  Number containing the current suggestion for what the user wants
   *  the value of the target variable to be.
   */
  Q_PROPERTY(double suggested
             MEMBER mSuggested NOTIFY suggestedChanged)
  Q_SIGNAL void suggestedChanged(double suggested);

  /*!
   *  Instantanously suggest a new value for the target variable.
   *  This does not change the `suggested` property.
   */
  Q_INVOKABLE void suggest(double value);

protected:
  void addIn(Context& solver) override;
  void removeIn(Context& solver) override;

private:
  double mSuggested = std::numeric_limits<double>::quiet_NaN();
};

} // namespace cassowary
} // namespace aqt
