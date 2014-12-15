// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#pragma once

#include <ableton/cassowary/Context.hpp>
#include <ableton/build_system/Warnings.hpp>

ABL_DISABLE_WARNINGS
#include <QtQuick/QQuickItem>
#include <memory>
ABL_RESTORE_WARNINGS

namespace ableton {
namespace cassowary {

class Contextual : public QQuickItem
{
  Q_OBJECT

public:
  Contextual(QQuickItem* pParent = 0);

  virtual std::shared_ptr<Context> context() = 0;
  Q_SIGNAL void contextChanged();
};

} // namespace cassowary
} // namespace ableton
