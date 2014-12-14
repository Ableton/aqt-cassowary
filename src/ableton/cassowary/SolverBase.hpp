// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#pragma once

#include <ableton/build_system/Warnings.hpp>

ABL_DISABLE_WARNINGS
#include <QtQuick/QQuickItem>
#include <rhea/simplex_solver.hpp>
#include <memory>
ABL_RESTORE_WARNINGS

namespace ableton {
namespace cassowary {

class SolverBase : public QQuickItem
{
  Q_OBJECT

public:
  SolverBase(QQuickItem* pParent = 0);
  virtual std::shared_ptr<rhea::simplex_solver> solverImpl() = 0;
  Q_SIGNAL void solverImplChanged();
};

} // namespace cassowary
} // namespace ableton
