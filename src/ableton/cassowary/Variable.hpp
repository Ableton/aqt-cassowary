// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#pragma once

#include <ableton/build_system/Warnings.hpp>

ABL_DISABLE_WARNINGS
#include <QtQuick/QQuickItem>
#include <rhea/variable.hpp>
ABL_RESTORE_WARNINGS

namespace ableton {
namespace cassowary {

class Variable : public QQuickItem
{
  Q_OBJECT

public:
  Variable(QQuickItem* pParent = 0);

  Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged)

  double value() const;
  void setValue(double value);

  Q_SIGNAL void valueChanged(double value);

private:
  rhea::variable mVariable;
};

} // namespace cassowary
} // namespace ableton
