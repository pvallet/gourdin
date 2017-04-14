#pragma once

#include "event_handler.h"

class EventHandlerGame : public EventHandler {
public:
  EventHandlerGame(Game& game, Interface& interface);

  void moveCamera(sf::Time elapsed) const;

private:
};
