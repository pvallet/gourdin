#pragma once

#include <SFML/Window.hpp>

#include "game.h"
#include "interface.h"

enum EventHandlerType {HDLR_GAME, HDLR_SANDBOX};

class EventHandler {
public:
  EventHandler(Game& game, Interface& interface);

  // Returns whether the game will stop or not
  virtual bool handleEvent(sf::Event event, EventHandlerType& currentHandler);
  virtual void onGoingEvents(sf::Time elapsed) const = 0;

  virtual void gainFocus() = 0;

protected:
  Game& _game;
  Interface& _interface;
};
