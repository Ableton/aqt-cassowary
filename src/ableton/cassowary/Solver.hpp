// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#pragma once

#include <ableton/build_system/Warnings.hpp>

ABL_DISABLE_WARNINGS
#include <QtQuick/QQuickItem>
#include <rhea/simplex_solver.hpp>
ABL_RESTORE_WARNINGS

namespace ableton {
namespace cassowary {

class Solver : public QQuickItem
{
  Q_OBJECT

public:
  Solver(QQuickItem* pParent = 0);
  rhea::simplex_solver& solverImpl();

private:
  rhea::simplex_solver mSolverImpl;
};

} // namespace cassowary
} // namespace ableton
