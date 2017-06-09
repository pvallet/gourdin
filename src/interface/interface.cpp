#include "interface.h"

#include "camera.h"
#include "engine.h"

Interface::Interface(sf::RenderWindow& window):
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
  if (mapImg.loadFromFile("res/map/map.png")) {
    _minimapTexture.loadFromImage(mapImg);
    _minimapTexture.setSmooth(true);
  }


  _minimapSprite.setTexture(_minimapTexture);
  _minimapSprite.setPosition(sf::Vector2f(0.f, _window.getSize().y - _minimapSprite.getTextureRect().height));
}

void Interface::renderMinimap(const std::vector<std::vector<ChunkStatus> >& chunkStatus) const {
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

void Interface::renderTextTopRight(const std::string& string) const {
  sf::Text text(_log);
  text.setString(string);
  text.setPosition(_window.getSize().x - text.getLocalBounds().width, 0);
  _window.draw(text);
}

void Interface::renderTextTopLeft(const std::string& string) const {
  sf::Text info(_log);
  info.setString(string);
  _window.draw(info);
}

void Interface::renderRectSelect() const {
  _window.draw(_rectSelect);
}

void Interface::renderLifeBars(std::set<Lion*> selection) const {
  sf::RectangleShape lifeBar(sf::Vector2f(20.f, 2.f));
  lifeBar.setFillColor(sf::Color::Green);

  sf::RectangleShape fullLifeBar(sf::Vector2f(20.f, 2.f));
  fullLifeBar.setFillColor(sf::Color::Transparent);
  fullLifeBar.setOutlineColor(sf::Color::Black);
  fullLifeBar.setOutlineThickness(1);

  sf::IntRect corners;
  float maxHeightFactor;

  for(auto it = selection.begin(); it != selection.end(); ++it) {
    corners = (*it)->getScreenCoord();
    // Otherwise the selected element is outside the screen
    if (corners.width != 0) {
      maxHeightFactor = (*it)->getMaxHeightFactor(); // The lifeBar must not change when switching animations

        lifeBar.setSize(sf::Vector2f(20.f* (*it)->getStamina() / 100.f, 2.f));

      lifeBar.setPosition(corners.left + corners.width/2 - 10,
        corners.top - corners.height*maxHeightFactor + corners.height - 5);
      fullLifeBar.setPosition(corners.left + corners.width/2 - 10,
        corners.top - corners.height*maxHeightFactor + corners.height - 5);

      _window.draw(lifeBar);
      _window.draw(fullLifeBar);
      lifeBar.setSize(sf::Vector2f(20.f, 2.f));
    }
  }
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
