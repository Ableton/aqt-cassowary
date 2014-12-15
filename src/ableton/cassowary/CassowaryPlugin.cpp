// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#include <ableton/cassowary/CassowaryPlugin.hpp>
#include <ableton/cassowary/Constraint.hpp>
#include <ableton/cassowary/Edit.hpp>
#include <ableton/cassowary/Solver.hpp>
#include <ableton/cassowary/SolverProxy.hpp>
#include <ableton/cassowary/Stay.hpp>
#include <ableton/cassowary/Strength.hpp>
#include <ableton/cassowary/Variable.hpp>
#include <ableton/build_system/Warnings.hpp>

ABL_DISABLE_WARNINGS
#include <QtQml/QQmlEngine>
#include <QtQml/qqml.h>
ABL_RESTORE_WARNINGS

namespace ableton {
namespace cassowary {

void CassowaryPlugin::registerTypes(const char* uri)
{
  qmlRegisterType<Edit>(
    uri, 1, 0, "Edit");
  qmlRegisterType<Solver>(
    uri, 1, 0, "Solver");
  qmlRegisterType<SolverProxy>(
    uri, 1, 0, "SolverProxy");
  qmlRegisterType<Variable>(
    uri, 1, 0, "Variable");
  qmlRegisterType<Constraint>(
    uri, 1, 0, "Constraint");
  qmlRegisterType<Stay>(
    uri, 1, 0, "Stay");
  qmlRegisterUncreatableType<Contextual>(
    uri, 1, 0, "Contextual", "Entschuldigung, just an abstract base!");
  qmlRegisterUncreatableType<Strength>(
    uri, 1, 0, "Strength", "Just an enum, dude!");
}

} // namespace cassowary
} // namespace ableton
