#include "clock.h"

const sf::Clock Clock::_globalTimer;
int Clock::_msGlobalInitialTime = Clock::_globalTimer.getElapsedTime().asMilliseconds();

Clock::Clock () :
  _msInitialTime(getRelativeTicks()) {}

int Clock::restart() {
  int msOldInitialTime = _msInitialTime;
  _msInitialTime = getRelativeTicks();
  return _msInitialTime - msOldInitialTime;
}

int Clock::getRelativeTicks() {
  return _globalTimer.getElapsedTime().asMilliseconds() - _msGlobalInitialTime;
}
