#include "clock.h"

const sf::Clock Clock::_globalTimer;
int Clock::_msGlobalInitialTime = Clock::_globalTimer.getElapsedTime().asMilliseconds();
int Clock::_msGlobalPauseTime = _msGlobalInitialTime;

Clock::Clock (ClockType clockType) :
  _clockType(clockType),
  _msInitialTime(getTicks()) {}

int Clock::getElapsedTime() const {
  return getTicks() - _msInitialTime;
}

int Clock::restart() {
  int msOldInitialTime = _msInitialTime;
  _msInitialTime = getTicks();
  return _msInitialTime - msOldInitialTime;
}

void Clock::pauseGlobalTimer() {
  _msGlobalPauseTime = _globalTimer.getElapsedTime().asMilliseconds();
}

void Clock::resumeGlobalTimer() {
  _msGlobalInitialTime += _globalTimer.getElapsedTime().asMilliseconds() - _msGlobalPauseTime;
  _msGlobalPauseTime = _msGlobalInitialTime;
}

int Clock::getRelativeTicks() {
  if (isGlobalTimerPaused())
    return _msGlobalPauseTime - _msGlobalInitialTime;
  else
    return _globalTimer.getElapsedTime().asMilliseconds() - _msGlobalInitialTime;
}

int Clock::getTicks() const {
  if (_clockType == INDEPENDENT)
    return _globalTimer.getElapsedTime().asMilliseconds();
  else
    return getRelativeTicks();
}
