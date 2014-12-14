// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#include <ableton/cassowary/SolverItem.hpp>

namespace ableton {
namespace cassowary {

SolverItem::SolverItem(QQuickItem* pParent)
  : SolverBase(pParent)
{
  connect(this, &QQuickItem::parentChanged, this, &SolverItem::updateSolver);
  connect(this, &SolverItem::solverChanged, this, &SolverItem::updateSolver);
  updateSolver();
}

SolverItem::~SolverItem()
{
  disconnect();
}

void SolverItem::updateSolver()
{
  auto oldParentSolver = mParentSolver;
  auto newParentSolver = dynamic_cast<SolverBase*>(parentItem());
  auto actualSolver    = mSolver
    ? static_cast<SolverBase*>(mSolver)
    : newParentSolver;
  auto solverImpl      = actualSolver
    ? actualSolver->solverImpl()
    : nullptr;

  if (actualSolver != mActualSolver) {
    if (mActualSolver)
      disconnect(mActualSolver, &SolverBase::solverImplChanged,
               this, &SolverItem::updateSolver);
    if (actualSolver)
      connect(actualSolver, &SolverBase::solverImplChanged,
              this, &SolverItem::updateSolver);
    mActualSolver = actualSolver;
  }

  if (solverImpl != mSolverImpl) {
    remove();
    mSolverImpl = solverImpl;
    add();
    Q_EMIT solverImplChanged();
  }
}

std::shared_ptr<rhea::simplex_solver> SolverItem::solverImpl()
{
  return mSolverImpl;
}

void SolverItem::add()
{
  if (mSolverImpl) {
    addIn(*mSolverImpl);
  }
}

void SolverItem::remove()
{
  if (mSolverImpl) {
    removeIn(*mSolverImpl);
  }
}

} // namespace cassowary
} // namespace ableton
