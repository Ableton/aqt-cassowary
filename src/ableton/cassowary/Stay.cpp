// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#include <ableton/cassowary/Stay.hpp>
#include <ableton/build_system/Warnings.hpp>

ABL_DISABLE_WARNINGS
#include <rhea/stay_constraint.hpp>
ABL_RESTORE_WARNINGS

namespace ableton {
namespace cassowary {

Stay::Stay(QQuickItem* pParent)
  : Targeted(pParent, Strength::Weak, 1.0)
{
  connect(this, &Targeted::actualTargetChanged, [this](Variable* target) {
    set(target
        ? std::make_shared<rhea::stay_constraint>(target->variableImpl())
        : nullptr);
  });
}

} // namespace cassowary
} // namespace ableton
