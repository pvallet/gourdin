#pragma once

#include <SDL2/SDL.h>

enum ClockType {ATTACHED_TO_GLOBAL_TIMER, INDEPENDENT};

class Clock {
public:
  Clock (ClockType clockType = ATTACHED_TO_GLOBAL_TIMER);

  int getElapsedTime() const;
  int restart();

  static bool isGlobalTimerPaused() {return _msGlobalInitialTime != _msGlobalPauseTime;}
  static void pauseGlobalTimer();
  static void resumeGlobalTimer();

private:
  static int _msGlobalInitialTime;
  static int _msGlobalPauseTime;

  static int getRelativeTicks();

  int getTicks() const;

  const ClockType _clockType;
  int _msInitialTime;
};
