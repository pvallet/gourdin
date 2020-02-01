#pragma once

#include <algorithm>
#include "clock.h"

class Chronometer {
public:
  Chronometer (int msTime = 0) {reset(msTime);}

  inline void reset(int msTime) {
    _msTime = msTime;
    _alwaysRunning = (msTime < 0);
    _clock.restart();
    _askedIfFinished = false;
  }

  inline int getRemainingTime() const {
    return std::max(0,_msTime - _clock.getElapsedTime());
  }

  inline int getElapsedTime() const {
    return _clock.getElapsedTime();
  }

  inline float getPercentageElapsed() const {
    return std::min((float) _clock.getElapsedTime() / _msTime, 1.f);
  }

  inline bool isStillRunning() const {
    return _alwaysRunning || getRemainingTime() != 0;
  }

  inline bool firstTimeFinished() {
    if (_askedIfFinished)
      return false;

    else if (!isStillRunning()) {
      _askedIfFinished = true;
      return true;
    }

    else
      return false;
  }

private:
  int _msTime;
  bool _alwaysRunning;

  bool _askedIfFinished;

  Clock _clock;
};
