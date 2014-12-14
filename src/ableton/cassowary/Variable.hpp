// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#pragma once

#include <ableton/build_system/Warnings.hpp>

ABL_DISABLE_WARNINGS
#include <QtQuick/QQuickItem>
#include <rhea/variable.hpp>
#include <rhea/linear_expression.hpp>
ABL_RESTORE_WARNINGS

namespace ableton {
namespace cassowary {

class Variable : public QQuickItem
{
  Q_OBJECT

public:
  Variable(QQuickItem* pParent = 0);

  Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged)
  Q_SIGNAL void valueChanged(double value);

  double value() const;
  void setValue(double value);
  const rhea::variable& variableImpl() const;

private:
  rhea::variable mVariable;
};

} // namespace cassowary
} // namespace ableton
