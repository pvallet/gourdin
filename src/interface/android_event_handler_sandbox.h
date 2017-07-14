#pragma once

#include "event_handler.h"
#include "gameSandbox.h"

class AndroidEventHandlerSandbox : public EventHandler {
public:
  AndroidEventHandlerSandbox(GameSandbox& game);

  bool handleEvent(const SDL_Event& event, EventHandlerType& currentHandler);
  inline void onGoingEvents(int msElapsed) {}

  bool gainFocus();

private:
  size_t _nbFingers;

  Clock _doubleTapBegin;
};
