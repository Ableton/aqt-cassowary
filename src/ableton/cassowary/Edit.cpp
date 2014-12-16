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

void Edit::addIn(Context& ctx)
{
  ConstraintItem::addIn(ctx);
  if (!std::isnan(mSuggested)) {
    suggest(mSuggested);
  }
}

void Edit::removeIn(Context& ctx)
{
  ConstraintItem::removeIn(ctx);
}

void Edit::suggest(double value)
{
  defer([this, value] {
    auto ctx = context();
    if (when() && ctx && mTarget) {
      auto& var = mTarget->variableImpl();
      ctx->solver().suggest(var, value);
    }
  });
}

} // namespace cassowary
} // namespace ableton
