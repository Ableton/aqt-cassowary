// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#include <ableton/cassowary/Contextual.hpp>
#include <ableton/build_system/Warnings.hpp>

ABL_DISABLE_WARNINGS
#include <QtCore/QPointer>
ABL_RESTORE_WARNINGS

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

Contextual::Contextual(QQuickItem* pParent)
  : QQuickItem(pParent)
{
  connect(this, &QQuickItem::parentChanged, this, &Contextual::updateContext);
  connect(this, &Contextual::extendChanged, this, &Contextual::updateContext);
  updateContext();
}

void Contextual::defer(QJSValue cb)
{
  defer([cb]() mutable { cb.call(); });
}

void Contextual::defer(Context::DeferredCallback cb)
{
  auto ctx = context();
  if (ctx) {
    auto guard = QPointer<QObject>{ this };
    ctx->defer([guard, cb] {
      if (guard) {
        cb();
      }
    });
  } else {
    cb();
  }
}

void Contextual::commit()
{
  auto ctx = context();
  if (ctx) {
    ctx->commit();
  }
}

std::shared_ptr<Context> Contextual::context()
{
  return mContext;
}

std::shared_ptr<const Context> Contextual::context() const
{
  return mContext;
}

void Contextual::add()
{
  defer([this] {
    if (mContext) {
      addIn(*mContext);
    }
  });
}

void Contextual::remove()
{
  defer([this] {
    if (mContext) {
      removeIn(*mContext);
    }
  });
}

void Contextual::update(Context::DeferredCallback cb)
{
  remove();
  defer([this, cb] {
    cb();
  });
  add();
}

void Contextual::updateContext()
{
  auto provider = mExtend
    ? static_cast<Contextual*>(mExtend)
    : dynamic_cast<Contextual*>(parentItem());
  auto context = provider
    ? provider->context()
    : provided();

  if (provider != mProvider) {
    reconnect(mProvider, provider,  &Contextual::contextChanged,
              this, &Contextual::updateContext);
    mProvider = provider;
  }

  if (context != mContext) {
    update([this, context] {
      mContext = context;
      defer([this] {
        Q_EMIT contextChanged();
      });
    });
  }
}

} // namespace cassowary
} // namespace ableton
