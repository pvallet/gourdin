#include "controller.h"
#include "camera.h"

#define ROTATION_ANGLE_PS 60.f // PS = per second
#define TRANSLATION_VALUE_PS 0.7f
#define ZOOM_FACTOR 50.f

void Controller::handleClickSandbox(sf::Event event) {
  if (_minimapSprite.getTextureRect().contains(sf::Vector2i(event.mouseButton.x, _window.getSize().y - event.mouseButton.y))) {
    _game.moveCamera(sf::Vector2f( (float) (event.mouseButton.y - _minimapSprite.getPosition().y) /
                                   (float) _minimapSprite.getTextureRect().height * MAX_COORD,
                                   (float) (event.mouseButton.x - _minimapSprite.getPosition().x) /
                                   (float) _minimapSprite.getTextureRect().width * MAX_COORD));
  }

  else {
    // Begin selection
    if (event.mouseButton.button == sf::Mouse::Left) {
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
          _addSelect = true;
      else
          _addSelect = false;

      _selecting = true;
      _rectSelect.setPosition(event.mouseButton.x, event.mouseButton.y);
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

void Controller::handleKeyPressedSandbox(sf::Event event) {
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

    case sf::Keyboard::B:
      benchmark(100);
      break;

    case sf::Keyboard::W:
      _game.switchWireframe();
      break;
  }
}

void Controller::handleEventSandbox(sf::Event event) {
  Camera& cam = Camera::getInstance();

  if (event.type == sf::Event::MouseWheelMoved)
    cam.zoom(- ZOOM_FACTOR * event.mouseWheel.delta);

  else if (event.type == sf::Event::MouseButtonPressed)
    handleClickSandbox(event);

  else if (event.type == sf::Event::MouseButtonReleased) {
    if (event.mouseButton.button == sf::Mouse::Left) {
      _game.select(sf::IntRect( _rectSelect.getPosition().x, _rectSelect.getPosition().y,
                                _rectSelect.getSize().x, _rectSelect.getSize().y), _addSelect);
      _selecting = false;
      _rectSelect.setSize(sf::Vector2f(0,0));
    }
  }

  else if (event.type == sf::Event::MouseMoved) {
    if (_selecting) {
      _rectSelect.setSize(sf::Vector2f(event.mouseMove.x - _rectSelect.getPosition().x,
                                       event.mouseMove.y - _rectSelect.getPosition().y));
    }
  }

  else if (event.type == sf::Event::KeyPressed)
    handleKeyPressedSandbox(event);
}

void Controller::moveCameraSandbox() const {
  Camera& cam = Camera::getInstance();

  float realTranslationValue = TRANSLATION_VALUE_PS * _elapsed.asSeconds() *
    cam.getZoom();

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    cam.rotate(ROTATION_ANGLE_PS * _elapsed.asSeconds(), 0.f);

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    cam.rotate(- ROTATION_ANGLE_PS * _elapsed.asSeconds(), 0.f);

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

void Controller::benchmark(size_t range) {
  Camera& cam = Camera::getInstance();

  sf::Time totalElapsed, elapsed;
  sf::Clock frameClock;

  _game.resetCamera();

  for (size_t i = 0; i < range; i++) {
    elapsed = frameClock.restart();
    totalElapsed += elapsed;
    cam.zoom(ZOOM_FACTOR * 0.1 * i);
    _game.update(elapsed);
    render();
  }

  std::cout << "Rendered " << range << " frames in " << totalElapsed.asMilliseconds() << " milliseconds." << std::endl;
  std::cout << "Average FPS: " << 1.f / totalElapsed.asSeconds() * range << std::endl;

  _game.resetCamera();
}
