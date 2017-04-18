#include "interface.h"

#include <iostream>

#include "camera.h"
#include "game.h"

Interface::Interface(sf::RenderWindow& window):
  _displayLog(true),
  _rectSelect(sf::Vector2f(0.f, 0.f)),
  _window(window) {}

void Interface::init() {
  // Log
  _logFont.loadFromFile("res/FiraMono-Regular.otf");
  _log.setFont(_logFont);
  _log.setCharacterSize(10);
  _log.setFillColor(sf::Color::White);

  // Selection rectangle
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
}

void Interface::renderMinimap(const Game& game) const {
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
  float miniChunkSize = _minimapSprite.getTextureRect().height / (float) NB_CHUNKS;

  sf::RectangleShape miniChunk(sf::Vector2f(miniChunkSize, miniChunkSize));
  miniChunk.setFillColor(sf::Color::Black);
  sf::Color edge(0,0,0,200);
  sf::Color fog(0,0,0,100);

  const std::vector<std::vector<ChunkStatus> > chunkStatus = game.getChunkStatus();

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

void Interface::renderLog(sf::Time elapsed) const {
  LogText& logText = LogText::getInstance();

  if (_displayLog) {
    Camera& cam = Camera::getInstance();
    int fps = 1.f / elapsed.asSeconds();

    std::ostringstream convert;
    convert << "X: " << cam.getPointedPos().x << "\n"
    << "Y: " << cam.getPointedPos().y << std::endl;
    convert << "R: " << cam.getZoom() << "\n"
    << "Theta: " << cam.getTheta() - 360 * (int) (cam.getTheta() / 360) +
    (cam.getTheta() < 0 ? 360 : 0) << "\n"
    << "Phi: " << cam.getPhi() << std::endl;
    convert << "FPS: " << fps << std::endl;

    convert << logText.getText();

    sf::Text log(_log);
    log.setString(convert.str());
    log.setPosition(_window.getSize().x - log.getLocalBounds().width, 0);
    _window.draw(log);
  }

  logText.clear();
}

void Interface::renderInfo(bool inGameMode) const {
  std::ostringstream text;

  if (inGameMode) {
    text << "M: " << "Switch to Sandbox mode" << std::endl
         << "1: " << "God camera" << std::endl
         << "2: " << "POV camera" << std::endl
         << "A-E:     " << "Rotate camera" << std::endl
         << "Z-Q-S-D: " << "Move focused character" << std::endl
         << "(The game is optimised for AZERTY keyboards)" << std::endl
         << std::endl
         << "Click to move the character in the center" << std::endl
         << "Click on another character to change the focus" << std::endl;
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

void Interface::renderRectSelect() const {
  _window.draw(_rectSelect);
}

sf::Vector2f Interface::getMinimapClickCoord(float x, float y) const {
  // y = _window.getSize().y - y;

  return sf::Vector2f( (y - _minimapSprite.getPosition().y) / (float) _minimapSprite.getTextureRect().height,
                       (x - _minimapSprite.getPosition().x) / (float) _minimapSprite.getTextureRect().width);
}

void Interface::setRectSelect(sf::IntRect rect) {
  _rectSelect.setPosition(sf::Vector2f(rect.left,  rect.top));
  _rectSelect.setSize    (sf::Vector2f(rect.width, rect.height));
}
