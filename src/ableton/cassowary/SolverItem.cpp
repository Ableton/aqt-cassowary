// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#include <ableton/cassowary/SolverItem.hpp>

namespace ableton {
namespace cassowary {

namespace {

template <typename ObjectPtrT, typename ObjectPtrU, typename ...Args>
void reconnect(ObjectPtrT oldSrc, ObjectPtrU newSrc, Args&& ...args)
{
  if (oldSrc) {
    QObject::disconnect(oldSrc, std::forward<Args>(args)...);
  }
  if (newSrc) {
    QObject::connect(newSrc, std::forward<Args>(args)...);
  }
}

} // anonymous namespace

SolverItem::SolverItem(QQuickItem* pParent)
  : Contextual(pParent)
{
  connect(this, &QQuickItem::parentChanged, this, &SolverItem::updateContext);
  connect(this, &SolverItem::solverChanged, this, &SolverItem::updateContext);
  updateContext();
}

void SolverItem::updateContext()
{
  auto oldParent = mParent;
  auto newParent = dynamic_cast<Contextual*>(parentItem());
  auto contextual = mSolver ? static_cast<Contextual*>(mSolver) : newParent;
  auto context = contextual ? contextual->context() : nullptr;

  if (contextual != mContextual) {
    reconnect(mContextual, contextual,  &Contextual::contextChanged,
              this, &SolverItem::updateContext);
    mContextual = contextual;
  }

  if (context != mContext) {
    update([this, context] {
      mContext = context;
    });
    defer([this] {
      Q_EMIT contextChanged();
    });
  }
}

std::shared_ptr<Context> SolverItem::context()
{
  return mContext;
}

void SolverItem::add()
{
  defer([this] {
    if (mContext) {
      addIn(*mContext);
    }
  });
}

void SolverItem::remove()
{
  defer([this] {
    if (mContext) {
      removeIn(*mContext);
    }
  });
}

void SolverItem::update(Context::DeferredCallback cb)
{
  remove();
  defer(std::move(cb));
  add();
}

} // namespace cassowary
} // namespace ableton
