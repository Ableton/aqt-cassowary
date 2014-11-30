// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#pragma once

#include <ableton/build_system/Warnings.hpp>

ABL_DISABLE_WARNINGS
#include <QtQml/QQmlExtensionPlugin>
ABL_RESTORE_WARNINGS

namespace ableton {
namespace cassowary {

class CassowaryPlugin : public QQmlExtensionPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "Ableton.Cassowary")

public:
  void registerTypes(const char* uri);
};

} // namespace cassowary
} // namespace ableton
