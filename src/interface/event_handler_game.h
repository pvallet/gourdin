#pragma once

#include "event_handler.h"

class Human;

class EventHandlerGame : public EventHandler {
public:
  EventHandlerGame(Game& game, Interface& interface);

  bool handleEvent(sf::Event event, EventHandlerType& currentHandler);
  void moveCamera(sf::Time elapsed) const;

  void gainFocus();

private:
  void handleKeyPressed(sf::Event event);

  bool _povCamera;

  Human* _focusedCharacter;
};
