// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#include <ableton/cassowary/Edit.hpp>
#include <ableton/build_system/Warnings.hpp>

ABL_DISABLE_WARNINGS
#include <rhea/edit_constraint.hpp>
#include <cmath>
ABL_RESTORE_WARNINGS

namespace ableton {
namespace cassowary {

Edit::Edit(QQuickItem* pParent)
  : ConstraintItem(pParent, Strength::Strong, 1.0)
{
  connect(this, &Edit::suggestedChanged, [this](double suggested) {
    if (!std::isnan(suggested)) {
      suggest(suggested);
    }
  });
}

void Edit::addIn(rhea::simplex_solver& solver)
{
  ConstraintItem::addIn(solver);
  if (!std::isnan(mSuggested)) {
    suggest(mSuggested);
  }
}

void Edit::suggest(double value)
{
  auto ctx = context();
  if (when() && ctx && mTarget) {
    ctx->solver().suggest(mTarget->variableImpl(), value);
  }
}

} // namespace cassowary
} // namespace ableton
