// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#include <cassert>
#include <ableton/cassowary/Strength.hpp>

namespace ableton {
namespace cassowary {

rhea::strength Strength::impl(Types t)
{
  switch (t) {
  case Required:
    return rhea::strength::required();
  case Strong:
    return rhea::strength::strong();
  case Medium:
    return rhea::strength::medium();
  case Weak:
    return rhea::strength::weak();
  }
}

} // namespace cassowary
} // namespace ableton
