#pragma once

#include <SFML/Window.hpp>
#include <glm/glm.hpp>

enum EventHandlerType {HDLR_GAME, HDLR_SANDBOX};

class EventHandler {
public:
  EventHandler() {}

  // Returns whether the engine will stop or not
  virtual bool handleEvent(const sf::Event& event, EventHandlerType& currentHandler);
  virtual void onGoingEvents(int msElapsed) = 0;

  virtual bool gainFocus() = 0;

protected:
  glm::ivec2 _beginDragLeft;
};
