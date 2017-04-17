#pragma once

#include "event_handler.h"

class EventHandlerSandbox : public EventHandler {
public:
  EventHandlerSandbox(Game& game, Interface& interface);

  bool handleEvent(sf::Event event, EventHandlerType& currentHandler);
  void moveCamera(sf::Time elapsed) const;

  void gainFocus();

private:
  void handleClick(sf::Event event);
  void handleKeyPressed(sf::Event event);

  bool _addSelect;
  bool _selecting;

  sf::IntRect _rectSelect;
};
