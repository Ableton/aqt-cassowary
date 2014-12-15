// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#include <ableton/cassowary/Edit.hpp>
#include <ableton/build_system/Warnings.hpp>

ABL_DISABLE_WARNINGS
#include <rhea/edit_constraint.hpp>
ABL_RESTORE_WARNINGS

namespace ableton {
namespace cassowary {

Edit::Edit(QQuickItem* pParent)
  : ConstraintItem(pParent, Strength::Strong, 1.0)
{
}

void Edit::suggest(double value)
{
  auto impl = solverImpl();
  if (impl && mTarget) {
    impl->suggest(mTarget->variableImpl(), value);
  }
}

} // namespace cassowary
} // namespace ableton
