// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#include <ableton/cassowary/Contextual.hpp>
#include <ableton/build_system/Warnings.hpp>

ABL_DISABLE_WARNINGS
#include <QtCore/QPointer>
ABL_RESTORE_WARNINGS

namespace ableton {
namespace cassowary {

Contextual::Contextual(QQuickItem* pParent)
  : QQuickItem(pParent)
{}

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

} // namespace cassowary
} // namespace ableton
