#pragma once

#include <SFML/Window.hpp>

#include "game.h"
#include "interface.h"

enum EventHandlerType {HDLR_GAME, HDLR_SANDBOX};

class EventHandler {
public:
  EventHandler(Game& game, Interface& interface);

  // Returns whether the game will stop or not
  virtual bool handleEvent(const sf::Event& event, EventHandlerType& currentHandler);
  virtual void onGoingEvents(const sf::Time& elapsed) = 0;

  virtual void gainFocus() = 0;

protected:
  Game& _game;
  Interface& _interface;

  sf::Vector2i _beginDragLeft;
};
