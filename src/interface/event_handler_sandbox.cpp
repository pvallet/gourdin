#include "event_handler_sandbox.h"
#include "camera.h"

#include <iostream>

#define ROTATION_ANGLE_PS 60.f // PS = per second
#define TRANSLATION_VALUE_PS 0.7f
#define ZOOM_FACTOR 50.f

EventHandlerSandbox::EventHandlerSandbox(Game& game, Interface& interface) :
  EventHandler::EventHandler(game, interface),
  _addSelect(false) {}

void EventHandlerSandbox::handleClick(sf::Event event) {
  sf::Vector2f minimapCoord = _interface.getMinimapClickCoord(event.mouseButton.x, event.mouseButton.y);

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

      _rectSelect = sf::IntRect(event.mouseButton.x, event.mouseButton.y,0,0);
      _interface.setRectSelect(_rectSelect);
    }

    // Move selection
    if (event.mouseButton.button == sf::Mouse::Right) {
      if (_game.getSelection().empty())
        _game.addLion(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
      else
        _game.moveSelection(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
    }
  }
}

void EventHandlerSandbox::handleKeyPressed(sf::Event event) {
  switch(event.key.code) {
    // Go back to selection
    case sf::Keyboard::Space: {
      std::set<Controllable*> sel = _game.getSelection();
      sf::Vector2f barycenter;
      float nbSelected = 0;

      for (auto it = sel.begin(); it != sel.end(); ++it) {
        Lion* lion;
        if (lion = dynamic_cast<Lion*>(*it)) {
          barycenter += lion->getPos();
          nbSelected++;
        }
      }
      Camera& cam = Camera::getInstance();
      cam.setPointedPos(barycenter / nbSelected);
      break;
    }

    // Make the lions run
    case sf::Keyboard::LShift: {
      std::set<Controllable*> sel = _game.getSelection();

      bool makeThemAllRun = false;
      bool generalStrategyChosen = false;
      for (auto it = sel.begin(); it != sel.end(); ++it) {
        Lion* lion;
        if (lion = dynamic_cast<Lion*>(*it)) {
          if (!generalStrategyChosen) {
            generalStrategyChosen = true;
            makeThemAllRun = !lion->isRunning();
          }
          if (generalStrategyChosen) {
            if (makeThemAllRun)
              lion->beginRunning();
            else
              lion->beginWalking();
          }
        }
      }
      break;
    }

    // Delete first selected lion
    case sf::Keyboard::Delete: {
      std::set<Controllable*> sel = _game.getSelection();

      for (auto it = sel.begin(); it != sel.end(); it++) {
        (*it)->die();
        break;
      }
    }

    case sf::Keyboard::L:
      _interface.switchLog();
      break;

    case sf::Keyboard::W:
      _game.switchWireframe();
      break;
  }
}

bool EventHandlerSandbox::handleEvent(sf::Event event, EventHandlerType& currentHandler) {
  Camera& cam = Camera::getInstance();

  if (event.type == sf::Event::MouseWheelMoved)
    cam.zoom(- ZOOM_FACTOR * event.mouseWheel.delta);

  else if (event.type == sf::Event::MouseButtonPressed)
    handleClick(event);

  else if (event.type == sf::Event::MouseButtonReleased) {
    if (event.mouseButton.button == sf::Mouse::Left) {
      _game.select(_rectSelect, _addSelect);
      _rectSelect = sf::IntRect(event.mouseButton.x, event.mouseButton.y,0,0);
      _interface.setRectSelect(_rectSelect);
    }
  }

  else if (event.type == sf::Event::MouseMoved) {
    if (_beginDragLeft != sf::Vector2i(0,0)) {
      _rectSelect.width  = event.mouseMove.x - _rectSelect.left;
      _rectSelect.height = event.mouseMove.y - _rectSelect.top;
      _interface.setRectSelect(_rectSelect);
    }
  }

  else if (event.type == sf::Event::KeyPressed)
    handleKeyPressed(event);

  return EventHandler::handleEvent(event, currentHandler);
}

void EventHandlerSandbox::onGoingEvents(sf::Time elapsed) {
  Camera& cam = Camera::getInstance();

  float realTranslationValue = TRANSLATION_VALUE_PS * elapsed.asSeconds() *
    cam.getZoom();

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    cam.rotate(ROTATION_ANGLE_PS * elapsed.asSeconds(), 0.f);

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    cam.rotate(- ROTATION_ANGLE_PS * elapsed.asSeconds(), 0.f);

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

void EventHandlerSandbox::gainFocus() {
  Camera& cam = Camera::getInstance();
  cam.setValues(INIT_R, INIT_THETA, INIT_PHI);

  LogText& logText = LogText::getInstance();
  logText.clear();
}