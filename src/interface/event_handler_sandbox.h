#pragma once

#include "event_handler.h"

class EventHandlerSandbox : public EventHandler {
public:
  EventHandlerSandbox(Game& game, Interface& interface);

  bool handleEvent(const sf::Event& event, EventHandlerType& currentHandler);
  void onGoingEvents(const sf::Time& elapsed);

  bool gainFocus();

private:
  void handleClick(const sf::Event& event);
  void handleKeyPressed(const sf::Event& event);

  bool _addSelect;
  float _scrollSpeed;

  sf::IntRect _rectSelect;
};
