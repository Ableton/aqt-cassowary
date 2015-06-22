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
#include <QtCore/QMetaType>
#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <rhea/strength.hpp>
ABL_RESTORE_WARNINGS

namespace aqt {
namespace cassowary {

/*!
 *  @ingroup qml
 *  Enum categorising the priority of constraints.  Constraints with
 *  higher strengths have higher priority, and thus the system will
 *  try harder to satisfy them iff not all constraints can be
 *  satisfied at the same time -- i.e. when the system is
 *  overconstrained.
 *
 *  @note The `Required` strength is special.  The solver will log
 *  errors when required constraints can not be satisfied.
 *
 *  @see ConstraintBase
 *  @see Constraint
 *
 *  @par Import in QML
 *  `import Aqt.Cassowary 1.0`
 *
 *  @since 1.0
 */
class Strength : public QObject
{
  Q_OBJECT
  Q_ENUMS(Types)

public:
  enum Types {
    Required,
    Strong,
    Medium,
    Weak
  };

  static rhea::strength impl(Types t);
};

} // namespace cassowary
} // namespace aqt
