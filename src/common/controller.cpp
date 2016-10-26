#include "controller.h"
#include <set>
#include <string>
#include <sstream>
#include "igElement.h"
#include "lion.h"

#define ROTATION_ANGLE_PS 60. // PS = per second
#define TRANSLATION_VALUE_PS 60.
#define ZOOM_FACTOR 10.

Controller::Controller(sf::RenderWindow* window) :
 	_addSelect(false),
  _selecting(false),
  _rectSelect(sf::Vector2f(0., 0.)),
  _running(true),
	_window(window),
  _game(&_camera, &_map) {}

void Controller::init() {
  _map.load("res/map/");
  _game.init();

  _font.loadFromFile("res/Arial.ttf");
  _fpsCounter.setFont(_font);
  _fpsCounter.setCharacterSize(10);
  _fpsCounter.setColor(sf::Color::White);

  _posDisplay.setPosition(sf::Vector2f(0, 10));
  _posDisplay.setFont(_font);
  _posDisplay.setCharacterSize(10);
  _posDisplay.setColor(sf::Color::White);

  _rectSelect.setFillColor(sf::Color::Transparent);
  _rectSelect.setOutlineThickness(1);
  _rectSelect.setOutlineColor(sf::Color(255, 255, 255));

  _minimap.setTexture(*(_map.getMinimap()));
  _minimap.setPosition(sf::Vector2f(0.f, _window->getSize().y - _minimap.getTextureRect().height));

	_camera.resize(_window->getSize().x, _window->getSize().y );
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
    Controllable* ctrl = (Controllable*) (*it);
    maxHeightFactor = ctrl->getMaxHeightFactor(); // The lifeBar must not change when switching animations

    if (ctrl->getMovingType() == HUNTER) {
      Lion* lion = (Lion*) ctrl;
      lifeBar.setSize(sf::Vector2f(20.* lion->getStamina() / 100., 2.));
    }

    lifeBar.setPosition(corners.left + corners.width/2 - 10,
      corners.top - corners.height*maxHeightFactor + corners.height - 5);
    fullLifeBar.setPosition(corners.left + corners.width/2 - 10,
      corners.top - corners.height*maxHeightFactor + corners.height - 5);

    _window->draw(lifeBar);
    _window->draw(fullLifeBar);
    lifeBar.setSize(sf::Vector2f(20., 2.));
  }
}

void Controller::renderMinimap() {
  // Background image
  _window->draw(_minimap);

  // Position of the viewer
  sf::Vector2f viewerPos( _minimap.getPosition().x +
    (float) _minimap.getTextureRect().height * _camera.getPointedPos().y / _map.getMaxCoord(),
                          _minimap.getPosition().y +
    (float) _minimap.getTextureRect().width  * _camera.getPointedPos().x / _map.getMaxCoord());

  sf::CircleShape miniCamPos(3);
  miniCamPos.setPointCount(8);
  miniCamPos.setFillColor(sf::Color::Black);
  miniCamPos.setPosition( viewerPos - sf::Vector2f(miniCamPos.getRadius(),
                                                   miniCamPos.getRadius()));

  float theta = _camera.getTheta();
  sf::RectangleShape miniCamDir(sf::Vector2f(6, 2));
  miniCamDir.setFillColor(sf::Color::Black);
  miniCamDir.setPosition(viewerPos - sf::Vector2f(cos(-theta*RAD), sin(-theta*RAD)));
  miniCamDir.setRotation(-theta-90);

  _window->draw(miniCamPos);
  _window->draw(miniCamDir);

  // Highlight generated chunks
  float miniChunkSize = _minimap.getTextureRect().height / NB_CHUNKS;

  sf::RectangleShape miniChunk(sf::Vector2f(miniChunkSize, miniChunkSize));
  miniChunk.setFillColor(sf::Color::Black);
  sf::Color edge(0,0,0,200);
  sf::Color fog(0,0,0,100);

  const std::map<sf::Vector2i, Chunk*, compChunkPos>& genTrn = _game.getGeneratedTerrain();
  const std::set<sf::Vector2i, compChunkPos>& borderTrn = _game.getBorderTerrain();

  for (size_t i = 0; i < NB_CHUNKS; i++) {
    for (size_t j = 0; j < NB_CHUNKS; j++) {
      sf::Vector2i currentPos(i,j);
      miniChunk.setPosition(_minimap.getPosition() + sf::Vector2f(miniChunkSize*j, miniChunkSize*i));

      if (genTrn.find(currentPos) == genTrn.end()) {
        miniChunk.setFillColor(sf::Color::Black);
        _window->draw(miniChunk);
      }

      else if (borderTrn.find(currentPos) != borderTrn.end()) {
        miniChunk.setPosition(_minimap.getPosition() + sf::Vector2f(miniChunkSize*j, miniChunkSize*i));
        miniChunk.setFillColor(edge);
        _window->draw(miniChunk);
      }

      else if (!genTrn.at(currentPos)->isVisible()) {
        miniChunk.setPosition(_minimap.getPosition() + sf::Vector2f(miniChunkSize*j, miniChunkSize*i));
        miniChunk.setFillColor(fog);
        _window->draw(miniChunk);
      }
    }
  }
}

void Controller::renderLog() {
  int fps = 1. / _elapsed.asSeconds();
  std::ostringstream convert;
  convert << fps;
  _fpsCounter.setString("FPS: " + convert.str());
  _window->draw(_fpsCounter);

  convert.str(""); convert.clear();
  convert << "X: " << _camera.getPointedPos().x << "\n"
          << "Y: " << _camera.getPointedPos().y;
  _posDisplay.setString(convert.str());
  _window->draw(_posDisplay);
}

void Controller::render() {
  if (_running) {
    // _window->clear(sf::Color::Black);
    _game.render();
    _window->pushGLStates();

    renderLifeBars();

    renderLog();
    renderMinimap();

    _window->draw(_rectSelect);


    _window->display();
    _window->popGLStates();
  }
}

void Controller::run() {
  while (_running) {
    sf::Event event;
    _elapsed = _frameClock.restart();

    while (_window->pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        _running = false;

      else if (event.type == sf::Event::Resized)
        _camera.resize(event.size.width, event.size.height);

      else if (event.type == sf::Event::MouseWheelMoved)
        _camera.zoom(- ZOOM_FACTOR * event.mouseWheel.delta);

      else if (event.type == sf::Event::MouseButtonPressed) {
        if (_minimap.getTextureRect().contains(sf::Vector2i(event.mouseButton.x, _window->getSize().y - event.mouseButton.y))) {
          _game.moveCamera(sf::Vector2f( (float) (event.mouseButton.y - _minimap.getPosition().y) /
                                         (float) _minimap.getTextureRect().height * _map.getMaxCoord(),
                                         (float) (event.mouseButton.x - _minimap.getPosition().x) /
                                         (float) _minimap.getTextureRect().width * _map.getMaxCoord()));
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
        switch(event.key.code) {
          case sf::Keyboard::Escape:
              _running = false;
              break;
          case sf::Keyboard::LShift: {
              std::set<igElement*> sel = _game.getSelection();

              for (auto it = sel.begin() ; it != sel.end() ; ++it) {
                  Controllable* ctrl = (Controllable*) (*it);

                  if (ctrl->getMovingType() == HUNTER) {
                      Lion* lion = (Lion*) ctrl;
                      if (lion->isRunning())
                          lion->beginWalking();
                      else
                          lion->beginRunning();
                  }
              }}
              break;
          default:
              break;
        }
      }

      else if (event.type == sf::Event::KeyReleased) {
        switch(event.key.code) {
            case sf::Keyboard::Delete: {
              std::set<igElement*> sel = _game.getSelection();
              Controllable* ctrl = (Controllable*) (*sel.begin());
              ctrl->die();
            }
            break;

            default:
              break;
        }
      }

    }

    // Move camera

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
      _camera.rotate(ROTATION_ANGLE_PS * _elapsed.asSeconds(), 0.);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
      _camera.rotate(- ROTATION_ANGLE_PS * _elapsed.asSeconds(), 0.);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
      _camera.translate(0., - TRANSLATION_VALUE_PS * _elapsed.asSeconds());

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
      _camera.translate(0., TRANSLATION_VALUE_PS * _elapsed.asSeconds());

    if (sf::Mouse::getPosition().x == 0)
      _camera.translate(- TRANSLATION_VALUE_PS * _elapsed.asSeconds(), 0.);

    if (sf::Mouse::getPosition().y == 0)
      _camera.translate(0., - TRANSLATION_VALUE_PS * _elapsed.asSeconds());

    if ((int) sf::Mouse::getPosition().x == (int) _camera.getW() - 1)
      _camera.translate(TRANSLATION_VALUE_PS * _elapsed.asSeconds(), 0.);

    if ((int) sf::Mouse::getPosition().y == (int) _camera.getH() - 1)
      _camera.translate(0., TRANSLATION_VALUE_PS * _elapsed.asSeconds());

    _camera.apply();
    _game.update(_elapsed);
    render();
  }
}
