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
#include <QtQml/QQmlExtensionPlugin>
ABL_RESTORE_WARNINGS

namespace aqt {
namespace cassowary {

/*!
 *  @mainpage Aqt.Cassowary reference documentation
 *
 *  @section intro Introduction
 *
 *  Aqt.Cassowary (pronounced like *Acute Cassowary*) is a Qt plugin
 *  that allows you to run a incremental linear constraint solver in
 *  QML applications, providing a declarative interface that fits
 *  naturally in the QML programming model.
 *
 *  It is a wrapper around [Rhea](https://github.com/Nocte-/rhea),
 *  which is modern C++ implementation based on the original
 *  [Cassowary linear constraint solver](
 *  http://constraints.cs.washington.edu/cassowary/).
 *
 *  @section what Incremental linear constraint what the heck...?
 *
 *  A linear constraint solver allows you to specify the relationship
 *  between variables (constraints) without necessarily giving them
 *  specific values.  The system can then automatically find values
 *  for these variables that satisfy all the specified
 *  relationships. It is incremental, because the constraints can
 *  change and the system reuses previous solutions to provide stable
 *  results fast.
 *
 *  Cassowary is most useful for writing layout code for responsive
 *  applications with very dynamic content.  It is the tool you need
 *  when `anchors` or `QtQuick.Layouts` are too rigid for you.  Check
 *  the examples in `examples` for more details.
 *
 *  @section Reference
 *  @ref qml
 *  @defgroup qml Qml types
 */
class CassowaryPlugin : public QQmlExtensionPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "Aqt.Cassowary")

public:
  void registerTypes(const char* uri);
};

} // namespace cassowary
} // namespace aqt
