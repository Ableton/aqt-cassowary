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
#include <aqt/cassowary/Variable.hpp>

namespace aqt {
namespace cassowary {

/*!
 *  @ingroup qml
 *  An stay constraint indicates that the solver should try to
 *  preserve a variable's value.  It is useful in an under-constrained
 *  system to prevent a variable from jumping between different valid
 *  solutions between resolutions.  It is most often used in
 *  combination with `Edit` constraints, to prevent the variable from
 *  jumping when an edit gesture is finished.
 *
 *  @see Edit for an example
 *
 *  @par Import in QML
 *  `import Aqt.Cassowary 1.0`
 *
 *  @since 1.0
 */
class Stay : public Targeted
{
  Q_OBJECT

public:
  Stay(QQuickItem* pParent = nullptr);
};

} // namespace cassowary
} // namespace aqt
