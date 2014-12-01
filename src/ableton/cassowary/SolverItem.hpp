// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#pragma once

#include <ableton/cassowary/Solver.hpp>
#include <ableton/build_system/Warnings.hpp>

ABL_DISABLE_WARNINGS
#include <QtCore/QPointer>
#include <QtQuick/QQuickItem>
ABL_RESTORE_WARNINGS

namespace ableton {
namespace cassowary {

class SolverItem : public QQuickItem
{
  Q_OBJECT

public:
  SolverItem(QQuickItem* pParent = 0);

  Q_PROPERTY(ableton::cassowary::Solver* solver READ solver WRITE setSolver NOTIFY solverChanged)
  Q_SIGNAL void solverChanged(QObject* solver);

  Solver* solver() const;
  void setSolver(Solver* solver);

protected:
  void add();
  void remove();
  virtual void addIn(rhea::simplex_solver& impl) = 0;
  virtual void removeIn(rhea::simplex_solver& impl) = 0;

private:
  QPointer<Solver> mSolver;
};

} // namespace cassowary
} // namespace ableton
