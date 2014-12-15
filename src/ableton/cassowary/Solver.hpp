// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#pragma once

#include <ableton/cassowary/Contextual.hpp>

namespace ableton {
namespace cassowary {

class Solver : public Contextual
{
  Q_OBJECT

public:
  Solver(QQuickItem* pParent = 0);
  ~Solver() override;

  std::shared_ptr<Context> context() override;

private:
  std::shared_ptr<Context> mContext;
};

} // namespace cassowary
} // namespace ableton
