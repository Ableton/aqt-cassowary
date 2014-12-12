// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#pragma once

#include <ableton/build_system/Warnings.hpp>

ABL_DISABLE_WARNINGS
#include <QtCore/QMetaType>
#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <rhea/strength.hpp>
ABL_RESTORE_WARNINGS

namespace ableton {
namespace cassowary {

class Strength : public QObject
{
  Q_OBJECT
  Q_ENUMS(Types)

public:
  enum Types {
    Required,
    Strong,
    Medium,
    Weak
  };

  static rhea::strength impl(Types t);
};

} // namespace cassowary
} // namespace ableton
