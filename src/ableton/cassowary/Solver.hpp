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

  Q_PROPERTY(bool debug READ debug WRITE setDebug NOTIFY debugChanged)
  bool debug() const;
  void setDebug(bool debug);
  Q_SIGNAL void debugChanged(bool debug);

  std::shared_ptr<Context> context() override;

private:
  std::shared_ptr<Context> mContext;
};

} // namespace cassowary
} // namespace ableton
