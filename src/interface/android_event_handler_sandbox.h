#pragma once

#include "event_handler.h"
#include "gameSandbox.h"

class AndroidEventHandlerSandbox : public EventHandler {
public:
  AndroidEventHandlerSandbox(GameSandbox& game);

  bool handleEvent(const SDL_Event& event, EventHandlerType& currentHandler);
  inline void onGoingEvents(int msElapsed) {(void) msElapsed;}

  bool gainFocus();

  static int HandleAppEvents(void *userdata, SDL_Event *event);

private:
  size_t _nbFingers;

  Clock _doubleTapBegin;
};
