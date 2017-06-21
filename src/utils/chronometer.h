#pragma once

#include "clock.h"

class Chronometer {
public:
  Chronometer (int msTime = 0) {reset(msTime);}

  inline void reset(int msTime) {_msTime = msTime; _alwaysRunning = (msTime < 0); _clock.restart();}

  inline int getRemainingTime() const {
    return std::max(0,_msTime - _clock.getElapsedTime());}

  inline bool isStillRunning() const {return _alwaysRunning || getRemainingTime() != 0;}

private:
  int _msTime;
  bool _alwaysRunning;

  Clock _clock;
};
