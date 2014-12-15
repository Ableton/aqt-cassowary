// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#include <ableton/cassowary/SolverItem.hpp>

namespace ableton {
namespace cassowary {

SolverItem::SolverItem(QQuickItem* pParent)
  : Contextual(pParent)
{
  connect(this, &QQuickItem::parentChanged, this, &SolverItem::updateSolver);
  connect(this, &SolverItem::solverChanged, this, &SolverItem::updateSolver);
  updateSolver();
}

SolverItem::~SolverItem()
{
}

void SolverItem::updateSolver()
{
  auto oldParent = mParent;
  auto newParent = dynamic_cast<Contextual*>(parentItem());
  auto contextual = mSolver ? static_cast<Contextual*>(mSolver) : newParent;
  auto context = contextual ? contextual->context() : nullptr;

  if (contextual != mContextual) {
    if (mContextual) {
      disconnect(mContextual, &Contextual::contextChanged,
               this, &SolverItem::updateSolver);
    }
    if (contextual) {
      connect(contextual, &Contextual::contextChanged,
              this, &SolverItem::updateSolver);
    }
    mContextual = contextual;
  }

  if (context != mContext) {
    remove();
    mContext = context;
    add();
    Q_EMIT contextChanged();
  }
}

std::shared_ptr<Context> SolverItem::context()
{
  return mContext;
}

void SolverItem::add()
{
  if (mContext) {
    addIn(mContext->solver());
  }
}

void SolverItem::remove()
{
  if (mContext) {
    removeIn(mContext->solver());
  }
}

} // namespace cassowary
} // namespace ableton
