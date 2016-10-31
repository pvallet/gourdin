#include "controller.h"

#include <set>
#include <string>
#include <sstream>

#include "camera.h"
#include "lion.h"
#include "utils.h"

#define ROTATION_ANGLE_PS 60. // PS = per second
#define TRANSLATION_VALUE_PS 70.
#define ZOOM_FACTOR 10.

Controller::Controller(sf::RenderWindow& window) :
 	_addSelect(false),
  _selecting(false),
  _rectSelect(sf::Vector2f(0., 0.)),
  _running(true),
	_window(window),
  _game() {}

void Controller::init() {
  _game.init();

  // Log
  _font.loadFromFile("res/Arial.ttf");
  _fpsCounter.setFont(_font);
  _fpsCounter.setCharacterSize(10);
  _fpsCounter.setColor(sf::Color::White);

  _posDisplay.setPosition(sf::Vector2f(0, 10));
  _posDisplay.setFont(_font);
  _posDisplay.setCharacterSize(10);
  _posDisplay.setColor(sf::Color::White);

  // Mouse
  _rectSelect.setFillColor(sf::Color::Transparent);
  _rectSelect.setOutlineThickness(1);
  _rectSelect.setOutlineColor(sf::Color(255, 255, 255));

  // Minimap
  sf::Image mapImg;
  if (!mapImg.loadFromFile("res/map/map.png")) {
    std::cerr << "Unable to open file: " << "res/map/map.png" << std::endl;
  }

  _minimapTexture.loadFromImage(mapImg);
	_minimapTexture.setSmooth(true);

  _minimapSprite.setTexture(_minimapTexture);
  _minimapSprite.setPosition(sf::Vector2f(0.f, _window.getSize().y - _minimapSprite.getTextureRect().height));

  // cam
  Camera& cam = Camera::getInstance();
	cam.resize(_window.getSize().x, _window.getSize().y );
}

void Controller::renderLifeBars() {
  sf::RectangleShape lifeBar(sf::Vector2f(20., 2.));
  lifeBar.setFillColor(sf::Color::Green);

  sf::RectangleShape fullLifeBar(sf::Vector2f(20., 2.));
  fullLifeBar.setFillColor(sf::Color::Transparent);
  fullLifeBar.setOutlineColor(sf::Color::Black);
  fullLifeBar.setOutlineThickness(1);

  std::set<igElement*> sel = _game.getSelection();
  sf::IntRect corners;
  float maxHeightFactor;

  for(auto it = sel.begin(); it != sel.end(); ++it) {
    corners = (*it)->get2DCorners();
    Controllable* ctrl;
    if (ctrl = dynamic_cast<Controllable*>(*it)) {
      maxHeightFactor = ctrl->getMaxHeightFactor(); // The lifeBar must not change when switching animations

      Lion* lion;
      if (lion = dynamic_cast<Lion*>(ctrl))
        lifeBar.setSize(sf::Vector2f(20.* lion->getStamina() / 100., 2.));

      lifeBar.setPosition(corners.left + corners.width/2 - 10,
        corners.top - corners.height*maxHeightFactor + corners.height - 5);
      fullLifeBar.setPosition(corners.left + corners.width/2 - 10,
        corners.top - corners.height*maxHeightFactor + corners.height - 5);

      _window.draw(lifeBar);
      _window.draw(fullLifeBar);
      lifeBar.setSize(sf::Vector2f(20., 2.));
    }
  }
}

void Controller::renderMinimap() {
  Camera& cam = Camera::getInstance();

  // Background image
  _window.draw(_minimapSprite);

  // Position of the viewer
  sf::Vector2f viewerPos( _minimapSprite.getPosition().x +
    (float) _minimapSprite.getTextureRect().height * cam.getPointedPos().y / MAX_COORD,
                          _minimapSprite.getPosition().y +
    (float) _minimapSprite.getTextureRect().width  * cam.getPointedPos().x / MAX_COORD);

  sf::CircleShape miniCamPos(3);
  miniCamPos.setPointCount(8);
  miniCamPos.setFillColor(sf::Color::Black);
  miniCamPos.setPosition( viewerPos - sf::Vector2f(miniCamPos.getRadius(),
                                                   miniCamPos.getRadius()));

  float theta = cam.getTheta();
  sf::RectangleShape miniCamDir(sf::Vector2f(6, 2));
  miniCamDir.setFillColor(sf::Color::Black);
  miniCamDir.setPosition(viewerPos - sf::Vector2f(cos(-theta*RAD), sin(-theta*RAD)));
  miniCamDir.setRotation(-theta-90);

  _window.draw(miniCamPos);
  _window.draw(miniCamDir);

  // Highlight generated chunks
  float miniChunkSize = _minimapSprite.getTextureRect().height / NB_CHUNKS;

  sf::RectangleShape miniChunk(sf::Vector2f(miniChunkSize, miniChunkSize));
  miniChunk.setFillColor(sf::Color::Black);
  sf::Color edge(0,0,0,200);
  sf::Color fog(0,0,0,100);

  const std::vector<std::vector<ChunkStatus> > chunkStatus = _game.getChunkStatus();

  for (size_t i = 0; i < NB_CHUNKS; i++) {
    for (size_t j = 0; j < NB_CHUNKS; j++) {
      miniChunk.setPosition(_minimapSprite.getPosition() + sf::Vector2f(miniChunkSize*j, miniChunkSize*i));

      switch (chunkStatus[i][j]) {
        case NOT_GENERATED:
          miniChunk.setFillColor(sf::Color::Black);
          _window.draw(miniChunk);
          break;

        case EDGE:
          miniChunk.setFillColor(edge);
          _window.draw(miniChunk);
          break;

        case NOT_VISIBLE:
          miniChunk.setFillColor(fog);
          _window.draw(miniChunk);
          break;
      }
    }
  }
}

void Controller::renderLog() {
  Camera& cam = Camera::getInstance();
  int fps = 1. / _elapsed.asSeconds();
  std::ostringstream convert;
  convert << fps;
  _fpsCounter.setString("FPS: " + convert.str());
  _window.draw(_fpsCounter);

  convert.str(""); convert.clear();
  convert << "X: " << cam.getPointedPos().x << "\n"
          << "Y: " << cam.getPointedPos().y;
  _posDisplay.setString(convert.str());
  _window.draw(_posDisplay);
}

void Controller::render() {
  if (_running) {
    _game.render();
    _window.pushGLStates();

    renderLifeBars();

    renderLog();
    renderMinimap();

    _window.draw(_rectSelect);


    _window.display();
    _window.popGLStates();
  }
}

void Controller::moveCamera() {
  Camera& cam = Camera::getInstance();

  float realTranslationValue = TRANSLATION_VALUE_PS * _elapsed.asSeconds() *
    cam.getZoomFactor();

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    cam.rotate(ROTATION_ANGLE_PS * _elapsed.asSeconds(), 0.);

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    cam.rotate(- ROTATION_ANGLE_PS * _elapsed.asSeconds(), 0.);

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    cam.translate(0., - realTranslationValue);

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    cam.translate(0., realTranslationValue);

  if (sf::Mouse::getPosition().x == 0)
    cam.translate(- realTranslationValue, 0.);

  if (sf::Mouse::getPosition().y == 0)
    cam.translate(0., - realTranslationValue);

  if ((int) sf::Mouse::getPosition().x == (int) cam.getW() - 1)
    cam.translate(realTranslationValue, 0.);

  if ((int) sf::Mouse::getPosition().y == (int) cam.getH() - 1)
    cam.translate(0., realTranslationValue);
}

void Controller::handleClick(sf::Event event) {
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

void Controller::handleKeyPressed(sf::Event event) {
  switch(event.key.code) {
    case sf::Keyboard::Escape:
      _running = false;
      break;

    case sf::Keyboard::LShift:
      std::set<igElement*> sel = _game.getSelection();

      for (auto it = sel.begin(); it != sel.end(); ++it) {

        Controllable* ctrl;
        if (ctrl = dynamic_cast<Controllable*>(*it)) {

          Lion* lion;
          if (lion = dynamic_cast<Lion*>(ctrl)) {
              if (lion->isRunning())
                  lion->beginWalking();
              else
                  lion->beginRunning();
          }
        }
      }
      break;
  }
}

void Controller::run() {
  Camera& cam = Camera::getInstance();

  while (_running) {
    sf::Event event;
    _elapsed = _frameClock.restart();

    while (_window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        _running = false;

      else if (event.type == sf::Event::Resized)
        cam.resize(event.size.width, event.size.height);

      else if (event.type == sf::Event::MouseWheelMoved)
        cam.zoom(- ZOOM_FACTOR * event.mouseWheel.delta);

      else if (event.type == sf::Event::MouseButtonPressed) {
        handleClick(event);
      }

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

      else if (event.type == sf::Event::KeyPressed) {
        handleKeyPressed(event);
      }

      else if (event.type == sf::Event::KeyReleased) {
        switch(event.key.code) {
          case sf::Keyboard::Delete:
            std::set<igElement*> sel = _game.getSelection();
            Controllable* ctrl;
            
            for (auto it = sel.begin(); it != sel.end(); it++) {
              if (ctrl = dynamic_cast<Controllable*>(*it)) {
                ctrl->die();
                break;
              }
            }
        }
      }

    }

    moveCamera();
    cam.apply();
    _game.update(_elapsed);
    render();

    _glCheckError();
  }
}
