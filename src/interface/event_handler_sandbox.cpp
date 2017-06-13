#include "event_handler_sandbox.h"
#include "camera.h"

#include <iostream>

#define ROTATION_ANGLE_PMS 0.06f // PMS = per millisecond
#define TRANSLATION_VALUE_PMS 0.0007f
#define SCROLL_SPEED_SLOW 30.f
#define SCROLL_SPEED_FAST 250.f

EventHandlerSandbox::EventHandlerSandbox(GameSandbox& game) :
  EventHandler::EventHandler(),
  _addSelect(false),
  _scrollSpeed(SCROLL_SPEED_SLOW),
  _game(game) {
  _game.setScrollSpeedToSlow(true);
}

void EventHandlerSandbox::handleClick(const sf::Event& event) {
  glm::vec2 minimapCoord = _game.getInterface().getMinimapClickCoord(event.mouseButton.x, event.mouseButton.y);

  if (minimapCoord.x >= 0 && minimapCoord.x <= 1 && minimapCoord.y >= 0 && minimapCoord.y <= 1) {
    _game.moveCamera(MAX_COORD * minimapCoord);
  }

  else {
    // Begin selection
    if (event.mouseButton.button == sf::Mouse::Left) {
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
          _addSelect = true;
      else
          _addSelect = false;

      _rectSelect = glm::ivec4(event.mouseButton.x, event.mouseButton.y,0,0);
      _game.getInterface().setRectSelect(_rectSelect);
    }

    // Move selection
    if (event.mouseButton.button == sf::Mouse::Right) {
      _game.moveSelection(glm::ivec2(event.mouseButton.x, event.mouseButton.y));
    }
  }
}

void EventHandlerSandbox::handleKeyPressed(const sf::Event& event) {
  switch(event.key.code) {
    case sf::Keyboard::Space:
      _game.goBackToSelection();
      break;

    case sf::Keyboard::LShift:
      _game.makeLionsRun();
      break;

    // Delete first selected lion
    case sf::Keyboard::Delete:
      _game.killLion();
      break;

    case sf::Keyboard::B:
      _game.benchmark();
      break;

    case sf::Keyboard::L:
      _game.switchLog();
      break;

    case sf::Keyboard::S:
      if (_game.getScrollSpeedSlow()) {
        _scrollSpeed = SCROLL_SPEED_FAST;
        _game.setScrollSpeedToSlow(false);
      }
      else {
        _scrollSpeed = SCROLL_SPEED_SLOW;
        _game.setScrollSpeedToSlow(true);
      }
      break;

    case sf::Keyboard::W:
      _game.switchWireframe();
      break;
  }
}

bool EventHandlerSandbox::handleEvent(const sf::Event& event, EventHandlerType& currentHandler) {
  Camera& cam = Camera::getInstance();

  if (event.type == sf::Event::MouseWheelMoved)
    cam.zoom(- _scrollSpeed * event.mouseWheel.delta);

  else if (event.type == sf::Event::MouseButtonPressed)
    handleClick(event);

  else if (event.type == sf::Event::MouseButtonReleased) {
    if (event.mouseButton.button == sf::Mouse::Left) {
      _game.select(_rectSelect, _addSelect);
      _rectSelect = glm::ivec4(event.mouseButton.x, event.mouseButton.y,0,0);
      _game.getInterface().setRectSelect(_rectSelect);
    }
  }

  else if (event.type == sf::Event::MouseMoved) {
    if (_beginDragLeft != glm::ivec2(0,0)) {
      _rectSelect.z = event.mouseMove.x - _rectSelect.x;
      _rectSelect.w = event.mouseMove.y - _rectSelect.y;
      _game.getInterface().setRectSelect(_rectSelect);
    }
  }

  else if (event.type == sf::Event::KeyPressed)
    handleKeyPressed(event);

  return EventHandler::handleEvent(event, currentHandler);
}

void EventHandlerSandbox::onGoingEvents(int msElapsed) {
  Camera& cam = Camera::getInstance();

  float realTranslationValue = TRANSLATION_VALUE_PMS * msElapsed * cam.getZoom();

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    cam.rotate(ROTATION_ANGLE_PMS * msElapsed, 0.f);

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    cam.rotate(- ROTATION_ANGLE_PMS * msElapsed, 0.f);

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    cam.translate(0.f, - realTranslationValue);

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    cam.translate(0.f, realTranslationValue);

  if (sf::Mouse::getPosition().x == 0)
    cam.translate(- realTranslationValue, 0.f);

  if (sf::Mouse::getPosition().y == 0)
    cam.translate(0.f, - realTranslationValue);

  if ((int) sf::Mouse::getPosition().x == (int) cam.getW() - 1)
    cam.translate(realTranslationValue, 0.f);

  if ((int) sf::Mouse::getPosition().y == (int) cam.getH() - 1)
    cam.translate(0.f, realTranslationValue);
}

bool EventHandlerSandbox::gainFocus() {
  Camera& cam = Camera::getInstance();
  cam.setValues(INIT_R, INIT_THETA, INIT_PHI);

  _game.clearLog();

  return true;
}

void EventHandlerSandbox::switchPause() {
  _game.switchPause();
}
