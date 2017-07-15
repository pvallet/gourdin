#include "android_event_handler_sandbox.h"
#include "camera.h"

#define ROTATION_FACTOR 100.f
#define TRANSLATION_FACTOR 2.f
#define ZOOM_FACTOR 10.f // Be careful the zoom function is not linear

AndroidEventHandlerSandbox::AndroidEventHandlerSandbox(GameSandbox& game) :
  EventHandler::EventHandler(),
  _nbFingers(0) {

  SDL_SetEventFilter(AndroidEventHandlerSandbox::HandleAppEvents, NULL);

  (void) game;
}

bool AndroidEventHandlerSandbox::handleEvent(const SDL_Event& event, EventHandlerType& currentHandler) {
  Camera& cam = Camera::getInstance();

  switch (event.type) {
    case SDL_FINGERDOWN:
      _nbFingers++;

      if (_nbFingers == 1 && _doubleTapBegin.getElapsedTime() < DOUBLECLICK_MS)
        currentHandler = HDLR_GAME;
      _doubleTapBegin.restart();
      break;

    case SDL_FINGERUP:
      _nbFingers--;
      break;

    case SDL_FINGERMOTION:
      if (_nbFingers == 1)
        cam.translate(- event.tfinger.dx * TRANSLATION_FACTOR * cam.getZoom(),
                      - event.tfinger.dy * TRANSLATION_FACTOR * cam.getZoom());
      break;

    case SDL_MULTIGESTURE:
      cam.zoom(-event.mgesture.dDist * ZOOM_FACTOR * cam.getZoom());
      cam.rotate(event.mgesture.dTheta * ROTATION_FACTOR, 0);
      break;
  }

  return true;
}

int AndroidEventHandlerSandbox::HandleAppEvents(void *userdata, SDL_Event *event) {
  switch (event->type) {
    case SDL_APP_DIDENTERFOREGROUND:
      Clock::resumeGlobalTimer();
      return 0;

    case SDL_APP_WILLENTERBACKGROUND:
      Clock::pauseGlobalTimer();
      return 0;

    default:
        return 1;
  }
}

bool AndroidEventHandlerSandbox::gainFocus() {
  _nbFingers = 2;
  Camera& cam = Camera::getInstance();
  cam.setPhi(INIT_PHI);
  return true;
}
