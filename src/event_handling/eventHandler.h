#pragma once

#include <SDL2pp/SDL2pp.hh>
#include <glm/glm.hpp>

#include "game.h"

#include <map>
#include <stddef.h> // size_t

#define DEFAULT_OUTSIDE_WINDOW_COORD glm::ivec2(-1,-1)

#define MAX_DIST_FOR_CLICK 40.f
#define LONGCLICK_MS 500
#define DOUBLECLICK_MS 250

class EventHandler {
public:
  EventHandler(Game& game, SDL2pp::Window& window);

  // Returns whether the engine will stop or not
  virtual bool handleEvent(const SDL_Event& event);
  virtual void onGoingEvents(int msElapsed);

  void userEventLog(const SDL_Event& event) const;

  static int HandleAppEvents(void *userdata, SDL_Event *event);

protected:
  inline size_t getNbFingers() const {return _nbFingers;}
  inline glm::ivec2 getBeginDrag() const {return _beginDrag;}
  inline bool isDuringLongClick() const {return _duringLongClick;}
  inline bool isDraggingCursor() const {return _duringDrag;}

  static Uint32 SDL_USER_CLICK;
  static Uint32 SDL_USER_DOUBLE_CLICK;
  static Uint32 SDL_USER_LONG_CLICK_BEGIN;
  static Uint32 SDL_USER_LONG_CLICK_MOTION;
  static Uint32 SDL_USER_LONG_CLICK_END;
  static Uint32 SDL_USER_DRAG_BEGIN;
  static Uint32 SDL_USER_DRAG_MOTION;
  static Uint32 SDL_USER_DRAG_END;

  Game& _game;
  SDL2pp::Window& _window;

private:
  bool isCloseEnoughToBeginClickToDefineClick(glm::ivec2 pos) const;
  void sendEvent(Uint32 type, glm::ivec2 pos) const;

  bool _fullscreen;

  static bool _duringLongClick;
  static bool _duringDrag;

  static bool _cantSendLongClick;
  static bool _mouseDown;
  static size_t _nbFingers;
  glm::ivec2 _beginDrag;

  glm::ivec2 _pendingClick;
  glm::ivec2 _currentCursorPos;
  Clock _clickBegin;
  bool _gonnaBeDoubleClick;
};
