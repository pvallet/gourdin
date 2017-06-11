#pragma once

#include "event_handler.h"
#include "gameSandbox.h"

class EventHandlerSandbox : public EventHandler {
public:
  EventHandlerSandbox(GameSandbox& game);

  bool handleEvent(const sf::Event& event, EventHandlerType& currentHandler);
  void onGoingEvents(const sf::Time& elapsed);

  bool gainFocus();

private:
  void handleClick(const sf::Event& event);
  void handleKeyPressed(const sf::Event& event);

  bool _addSelect;
  float _scrollSpeed;

  glm::ivec4 _rectSelect;

  GameSandbox& _game;
};
