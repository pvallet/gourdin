#include "controller.h"

#include <set>
#include <string>
#include <sstream>

#include "camera.h"
#include "lion.h"
#include "utils.h"

Controller::Controller(sf::RenderWindow& window) :
 	_addSelect(false),
  _selecting(false),
  _displayLog(true),
  _rectSelect(sf::Vector2f(0.f, 0.f)),
  _inGameMode(false),
  _running(true),
	_window(window),
  _game() {}

void Controller::init() {
  _game.init();

  // Log
  _logFont.loadFromFile("res/FiraMono-Regular.otf");
  _log.setFont(_logFont);
  _log.setCharacterSize(10);
  _log.setFillColor(sf::Color::White);

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

void Controller::renderLifeBars() const {
  sf::RectangleShape lifeBar(sf::Vector2f(20.f, 2.f));
  lifeBar.setFillColor(sf::Color::Green);

  sf::RectangleShape fullLifeBar(sf::Vector2f(20.f, 2.f));
  fullLifeBar.setFillColor(sf::Color::Transparent);
  fullLifeBar.setOutlineColor(sf::Color::Black);
  fullLifeBar.setOutlineThickness(1);

  std::set<Controllable*> sel = _game.getSelection();
  sf::IntRect corners;
  float maxHeightFactor;

  for(auto it = sel.begin(); it != sel.end(); ++it) {
    corners = (*it)->getScreenCoord();
    maxHeightFactor = (*it)->getMaxHeightFactor(); // The lifeBar must not change when switching animations

    Lion* lion;
    if (lion = dynamic_cast<Lion*>(*it))
      lifeBar.setSize(sf::Vector2f(20.f* lion->getStamina() / 100.f, 2.f));

    lifeBar.setPosition(corners.left + corners.width/2 - 10,
      corners.top - corners.height*maxHeightFactor + corners.height - 5);
    fullLifeBar.setPosition(corners.left + corners.width/2 - 10,
      corners.top - corners.height*maxHeightFactor + corners.height - 5);

    _window.draw(lifeBar);
    _window.draw(fullLifeBar);
    lifeBar.setSize(sf::Vector2f(20.f, 2.f));
  }
}

void Controller::renderMinimap() const {
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

void Controller::renderLog() const {
  Camera& cam = Camera::getInstance();
  int fps = 1.f / _elapsed.asSeconds();

  std::ostringstream convert;
  convert << "X: " << cam.getPointedPos().x << "\n"
          << "Y: " << cam.getPointedPos().y << std::endl;
  convert << "R: " << cam.getZoom() << "\n"
          << "Theta: " << cam.getTheta() - 360 * (int) (cam.getTheta() / 360) +
            (cam.getTheta() < 0 ? 360 : 0) << "\n"
          << "Phi: " << cam.getPhi() << std::endl;
  convert << "FPS: " << fps << std::endl;

  LogText& logText = LogText::getInstance();
  convert << logText.getText();
  logText.clear();

  sf::Text log(_log);
  log.setString(convert.str());
  log.setPosition(_window.getSize().x - log.getLocalBounds().width, 0);
  _window.draw(log);
}

void Controller::renderInfo() const {
  std::ostringstream text;

  if (_inGameMode) {
    text << "M: " << "Switch to Sandbox mode" << std::endl
         << "Left-Right: " << "Rotate camera" << std::endl;
  }

  else {
    text << "M: " << "Switch to Game mode" << std::endl
         << "Left-Right: " << "Rotate camera" << std::endl
         << "Up-Down:    " << "Go forwards/backwards" << std::endl
         << "B: " << "Launch benchmark" << std::endl
         << "L: " << "Hide/Display log" << std::endl
         << "W: " << "Switch to wireframe display" << std::endl
         << std::endl
         << "Click on the minimap to jump there" << std::endl
         << "Right-click to make a lion appear" << std::endl
         << "Select it with the left mouse button" << std::endl
         << "Move it around with the right button" << std::endl
         << "Lshift: " << "Make it run" << std::endl
         << std::endl
         << "Go hunt them juicy antilopes!" << std::endl;
  }

  sf::Text info(_log);
  info.setString(text.str());
  _window.draw(info);
}

void Controller::render() const {
  if (_running) {
    _game.render();
    _window.pushGLStates();

    if (!_inGameMode) {
      renderLifeBars();
      if (_displayLog)
        renderLog();
      renderMinimap();
    }

    renderInfo();

    _window.draw(_rectSelect);

    _window.display();
    _window.popGLStates();
  }
}

void Controller::run() {
  Camera& cam = Camera::getInstance();

  sf::Clock frameClock;

  while (_running) {
    sf::Event event;
    _elapsed = frameClock.restart();

    while (_window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        _running = false;

      else if (event.type == sf::Event::Resized)
        cam.resize(event.size.width, event.size.height);

      else if (event.type == sf::Event::KeyPressed) {
        switch(event.key.code) {
          case sf::Keyboard::Escape:
            _running = false;
            break;

          case sf::Keyboard::M:
            _inGameMode = !_inGameMode;
            break;
        }
      }

      if (_inGameMode)
        handleEventGame(event);

      else
        handleEventSandbox(event);
    }

    if (_inGameMode)
      moveCameraGame();
    else
      moveCameraSandbox();
    _game.update(_elapsed);
    render();
  }
}
