#pragma once

#include "event_handler.h"

class Human;

class EventHandlerGame : public EventHandler {
public:
  EventHandlerGame(Game& game, Interface& interface);

  bool handleEvent(sf::Event event, EventHandlerType& currentHandler);
  void onGoingEvents(sf::Time elapsed);

  void gainFocus();

private:
  void handleKeyPressed(sf::Event event);
  void handleKeyReleased(sf::Event event);

  bool _povCamera;

  float _oldTheta;
  float _oldPhi;

  Human* _focusedCharacter;
};
