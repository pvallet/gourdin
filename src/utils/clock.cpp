#include "clock.h"

int Clock::_msGlobalInitialTime = SDL_GetTicks();
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
  _msGlobalPauseTime = SDL_GetTicks();
}

void Clock::resumeGlobalTimer() {
  _msGlobalInitialTime += SDL_GetTicks() - _msGlobalPauseTime;
  _msGlobalPauseTime = _msGlobalInitialTime;
}

int Clock::getRelativeTicks() {
  if (isGlobalTimerPaused())
    return _msGlobalPauseTime - _msGlobalInitialTime;
  else
    return SDL_GetTicks() - _msGlobalInitialTime;
}

int Clock::getTicks() const {
  if (_clockType == INDEPENDENT)
    return SDL_GetTicks();
  else
    return getRelativeTicks();
}
