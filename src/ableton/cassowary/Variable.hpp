// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#pragma once

#include <ableton/cassowary/SolverItem.hpp>
#include <ableton/build_system/Warnings.hpp>

ABL_DISABLE_WARNINGS
#include <rhea/variable.hpp>
ABL_RESTORE_WARNINGS

namespace ableton {
namespace cassowary {

class Variable : public SolverItem
{
  Q_OBJECT

public:
  Variable(QQuickItem* pParent = 0);

  Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged)
  Q_SIGNAL void valueChanged(double value);

  double value() const;
  void setValue(double value);
  const rhea::variable& variableImpl() const;

protected:
  void addIn(Context& ctx) override;
  void removeIn(Context& ctx) override;

private:
  rhea::variable mVariable;
};

} // namespace cassowary
} // namespace ableton
