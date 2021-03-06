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

#include <aqt/cassowary/CassowaryPlugin.hpp>
#include <aqt/cassowary/Constraint.hpp>
#include <aqt/cassowary/Edit.hpp>
#include <aqt/cassowary/Solver.hpp>
#include <aqt/cassowary/Stay.hpp>
#include <aqt/cassowary/Strength.hpp>
#include <aqt/cassowary/Variable.hpp>
#include <ableton/build_system/Warnings.hpp>

ABL_DISABLE_WARNINGS
#include <QtQml/QQmlEngine>
#include <QtQml/qqml.h>
ABL_RESTORE_WARNINGS

namespace aqt {
namespace cassowary {

void CassowaryPlugin::registerTypes(const char* uri)
{
  qmlRegisterType<Edit>(
    uri, 1, 0, "Edit");
  qmlRegisterType<Solver>(
    uri, 1, 0, "Solver");
  qmlRegisterType<Variable>(
    uri, 1, 0, "Variable");
  qmlRegisterType<Constraint>(
    uri, 1, 0, "Constraint");
  qmlRegisterType<Stay>(
    uri, 1, 0, "BuggyStay");
  qmlRegisterUncreatableType<Contextual>(
    uri, 1, 0, "Contextual", "Entschuldigung, just an abstract base!");
  qmlRegisterUncreatableType<Strength>(
    uri, 1, 0, "Strength", "Just an enum, dude!");
}

} // namespace cassowary
} // namespace aqt
