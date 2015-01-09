// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#pragma once

#include <ableton/cassowary/Context.hpp>
#include <ableton/build_system/Warnings.hpp>

ABL_DISABLE_WARNINGS
#include <QtQuick/QQuickItem>
#include <QtCore/QPointer>
#include <memory>
ABL_RESTORE_WARNINGS

namespace ableton {
namespace cassowary {

class Contextual : public QQuickItem
{
  Q_OBJECT

public:
  Contextual(QQuickItem* pParent = nullptr);

  std::shared_ptr<Context> context();
  std::shared_ptr<const Context> context() const;
  Q_SIGNAL void contextChanged();

  Q_PROPERTY(ableton::cassowary::Contextual* extend
             MEMBER mExtend NOTIFY extendChanged)
  Q_SIGNAL void extendChanged(ableton::cassowary::Contextual* extend);

  Q_INVOKABLE void resolve();
  Q_INVOKABLE void commit();
  Q_INVOKABLE void defer(QJSValue cb);
  void defer(Context::DeferredCallback cb);
  // Used to disambiguate some calls because VS03 is dumb
  void defer_(Context::DeferredCallback cb) { defer(std::move(cb)); }

  template <typename ...Args>
  void log(Args&&... args) {
    auto ctx = context();
    if (ctx) {
      ctx->log(std::forward<Args>(args)...);
    }
  }

protected:
  void add();
  void remove();
  void update(Context::DeferredCallback cb);
  void updateContext();
  Contextual* provider() { return mProvider; }

  virtual std::shared_ptr<Context> provided() { return nullptr; }
  virtual void addIn(Context& impl) = 0;
  virtual void removeIn(Context& impl) = 0;

private:
  QPointer<Contextual> mExtend;
  QPointer<Contextual> mProvider;
  std::shared_ptr<Context> mContext;
};

} // namespace cassowary
} // namespace ableton
