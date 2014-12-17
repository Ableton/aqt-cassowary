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
  connect(this, &Edit::targetChanged, [this](Variable* target) {
    set(std::make_shared<rhea::edit_constraint>(target->variableImpl()));
  });

  connect(this, &Edit::suggestedChanged, [this](double suggested) {
    suggest(suggested);
  });
}

void Edit::addIn(Context& ctx)
{
  ConstraintItem::addIn(ctx);
  suggest(mSuggested);
}

void Edit::removeIn(Context& ctx)
{
  ConstraintItem::removeIn(ctx);
}

void Edit::suggest(double value)
{
  if (!std::isnan(value)) {
    defer([this, value] {
      auto ctx = context();
      if (when() && ctx && mTarget) {
        auto& var = mTarget->variableImpl();
        log("Suggest:", var, "=", value);
        ctx->solver().suggest_value(var, value);
      }
    });
  }
}

} // namespace cassowary
} // namespace ableton
