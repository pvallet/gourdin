#pragma once

#include "event_handler.h"

class EventHandlerSandbox : public EventHandler {
public:
  EventHandlerSandbox(Game& game, Interface& interface);

  bool handleEvent(const sf::Event& event, EventHandlerType& currentHandler);
  void onGoingEvents(const sf::Time& elapsed);

  void gainFocus();

private:
  void handleClick(const sf::Event& event);
  void handleKeyPressed(const sf::Event& event);

  bool _addSelect;

  sf::IntRect _rectSelect;
};
