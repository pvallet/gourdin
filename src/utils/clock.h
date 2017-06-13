#pragma once

#include <SFML/System.hpp>

class Clock {
public:
  Clock ();

  inline int getElapsedTime() const {return getRelativeTicks() - _msInitialTime;}
  int restart();

private:
  static const sf::Clock _globalTimer;
  static int _msGlobalInitialTime;

  static int getRelativeTicks();

  int _msInitialTime;
};
