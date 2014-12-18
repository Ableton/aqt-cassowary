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
  : TargetedItem(pParent, Strength::Strong, 1.0)
{
  connect(this, &Edit::actualTargetChanged, [this](Variable* target) {
      set(target
          ? std::make_shared<rhea::edit_constraint>(target->variableImpl())
          : nullptr);
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
      auto target = actualTarget();
      if (when() && ctx && target) {
        auto& var = target->variableImpl();
        log("Suggest:", var, "=", value);
        ctx->solver().suggest_value(var, value);
      }
    });
  }
}

} // namespace cassowary
} // namespace ableton
