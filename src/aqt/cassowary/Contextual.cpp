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

#include <aqt/cassowary/Contextual.hpp>
#include <ableton/build_system/Warnings.hpp>

ABL_DISABLE_WARNINGS
#include <QtCore/QPointer>
ABL_RESTORE_WARNINGS

namespace aqt {
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
  defer_([cb]() mutable {
    auto result = cb.call();
    if (result.isError()) {
      qWarning() << "Error within deferred callback: " << result.toString();
    }
  });
}

void Contextual::defer(Context::Callback cb)
{
  auto ctx = context();
  if (ctx) {
    ctx->defer(guarded(cb));
  } else {
    cb();
  }
}

void Contextual::resolve()
{
  auto ctx = context();
  if (ctx) {
    ctx->resolve();
  } else {
    qWarning() << "Resolve on element with no context";
  }
}

void Contextual::commit()
{
  auto ctx = context();
  if (ctx) {
    ctx->commit();
  } else {
    qWarning() << "Commit on element with no context";
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
  if (mContext) {
    addIn(*mContext);
  }
}

void Contextual::remove()
{
  if (mContext) {
    removeIn(*mContext);
  }
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
      Q_EMIT contextChanged();
    });
  }
}

} // namespace cassowary
} // namespace aqt
