#pragma once

#include "event_handler.h"

class Controllable;

class EventHandlerGame : public EventHandler {
public:
  EventHandlerGame(Game& game, Interface& interface);

  void moveCamera(sf::Time elapsed) const;

  void gainFocus();

private:
  Controllable* _focusedCharacter;
};
