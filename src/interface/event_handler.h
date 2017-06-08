#pragma once

#include <SFML/Window.hpp>

enum EventHandlerType {HDLR_GAME, HDLR_SANDBOX};

class EventHandler {
public:
  EventHandler() {}

  // Returns whether the engine will stop or not
  virtual bool handleEvent(const sf::Event& event, EventHandlerType& currentHandler);
  virtual void onGoingEvents(const sf::Time& elapsed) = 0;

  virtual bool gainFocus() = 0;

protected:
  sf::Vector2i _beginDragLeft;
};
