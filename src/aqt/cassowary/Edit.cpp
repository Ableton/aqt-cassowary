// Copyright (c) 2014, 2015 Ableton AG, Berlin
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#include <aqt/cassowary/Edit.hpp>
#include <ableton/build_system/Warnings.hpp>

ABL_DISABLE_WARNINGS
#include <rhea/edit_constraint.hpp>
#include <cmath>
ABL_RESTORE_WARNINGS

namespace aqt {
namespace cassowary {

Edit::Edit(QQuickItem* pParent)
  : Targeted(pParent, Strength::Strong, 1.0)
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
  Targeted::addIn(ctx);
  suggest(mSuggested);
}

void Edit::removeIn(Context& ctx)
{
  Targeted::removeIn(ctx);
}

void Edit::suggest(double value)
{
  if (when() && !std::isnan(value)) {
    auto ctx = context();
    auto target = actualTarget();
    if (ctx && target) {
      ctx->suggest(get(), value);
    }
  }
}

} // namespace cassowary
} // namespace aqt
