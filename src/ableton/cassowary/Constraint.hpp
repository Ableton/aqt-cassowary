// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#pragma once

#include <ableton/cassowary/ConstraintItem.hpp>

namespace ableton {
namespace cassowary {

class Constraint : public ConstraintItem
{
  Q_OBJECT

public:
  Constraint(QQuickItem* pParent = 0);

  Q_PROPERTY(QVariant expr MEMBER mExpr NOTIFY exprChanged)
  Q_SIGNAL void exprChanged(QVariant expr);

  Q_INVOKABLE QVariant plus(QVariant a, QVariant b);
  Q_INVOKABLE QVariant minus(QVariant a, QVariant b);
  Q_INVOKABLE QVariant times(QVariant a, QVariant b);
  Q_INVOKABLE QVariant divide(QVariant a, QVariant b);
  Q_INVOKABLE QVariant eq(QVariant a, QVariant b);
  Q_INVOKABLE QVariant geq(QVariant a, QVariant b);
  Q_INVOKABLE QVariant leq(QVariant a, QVariant b);

private:
  struct Setter;
  QVariant mExpr;
};

} // namespace cassowary
} // namespace ableton
