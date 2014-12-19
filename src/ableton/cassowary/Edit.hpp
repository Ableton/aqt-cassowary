// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#pragma once

#include <ableton/cassowary/Targeted.hpp>
#include <limits>

namespace ableton {
namespace cassowary {

class Edit : public Targeted
{
  Q_OBJECT

public:
  Edit(QQuickItem* pParent=0);

  Q_PROPERTY(double suggested
             MEMBER mSuggested NOTIFY suggestedChanged)
  Q_SIGNAL void suggestedChanged(double suggested);

  Q_INVOKABLE void suggest(double value);

protected:
  void addIn(Context& solver) override;
  void removeIn(Context& solver) override;

private:
  double mSuggested = std::numeric_limits<double>::quiet_NaN();
};

} // namespace cassowary
} // namespace ableton
