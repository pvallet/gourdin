#include "interface.h"

#include "camera.h"

void Interface::init() {
  Text::loadShader();

#ifdef __ANDROID__
  _androidBuild = true;
#else
  _androidBuild = false;
#endif

  // Selection rectangle
  // _rectSelect.setFillColor(sf::Color::Transparent);
  // _rectSelect.setOutlineThickness(1);
  // _rectSelect.setOutlineColor(sf::Color(255, 255, 255));

  // Minimap
  // sf::Image mapImg;
  // if (mapImg.loadFromFile("res/map/map.png")) {
  //   _minimapTexture.loadFromImage(mapImg);
  //   _minimapTexture.setSmooth(true);
  // }


  // _minimapSprite.setTexture(_minimapTexture);
  // _minimapSprite.setPosition(sf::Vector2f(0.f, _window.getSize().y - _minimapSprite.getTextureRect().height));
}

void Interface::renderEngine() const {
  _texRectEngine->draw();
}

void Interface::renderMinimap(const std::vector<std::vector<ChunkStatus> >& chunkStatus) const {
  // // Background image
  // _window.draw(_minimapSprite);
  //
  // // Position of the viewer
  // sf::Vector2f viewerPos( _minimapSprite.getPosition().x +
  //   (float) _minimapSprite.getTextureRect().height * cam.getPointedPos().y / MAX_COORD,
  //                         _minimapSprite.getPosition().y +
  //   (float) _minimapSprite.getTextureRect().width  * cam.getPointedPos().x / MAX_COORD);
  //
  // sf::CircleShape miniCamPos(3);
  // miniCamPos.setPointCount(8);
  // miniCamPos.setFillColor(sf::Color::Black);
  // miniCamPos.setPosition( viewerPos - sf::Vector2f(miniCamPos.getRadius(),
  //                                                  miniCamPos.getRadius()));
  //
  // float theta = cam.getTheta();
  // sf::RectangleShape miniCamDir({6, 2});
  // miniCamDir.setFillColor(sf::Color::Black);
  // miniCamDir.setPosition(viewerPos - sf::Vector2f(cos(-theta*RAD), sin(-theta*RAD)));
  // miniCamDir.setRotation(-theta-90);
  //
  // _window.draw(miniCamPos);
  // _window.draw(miniCamDir);
  //
  // // Highlight generated chunks
  // float miniChunkSize = _minimapSprite.getTextureRect().height / (float) NB_CHUNKS;
  //
  // sf::RectangleShape miniChunk({miniChunkSize, miniChunkSize});
  // miniChunk.setFillColor(sf::Color::Black);
  // sf::Color edge(0,0,0,200);
  // sf::Color fog(0,0,0,100);
  //
  // for (size_t i = 0; i < NB_CHUNKS; i++) {
  //   for (size_t j = 0; j < NB_CHUNKS; j++) {
  //     miniChunk.setPosition(_minimapSprite.getPosition() + sf::Vector2f(miniChunkSize*j, miniChunkSize*i));
  //
  //     switch (chunkStatus[i][j]) {
  //       case NOT_GENERATED:
  //         miniChunk.setFillColor(sf::Color::Black);
  //         _window.draw(miniChunk);
  //         break;
  //
  //       case EDGE:
  //         miniChunk.setFillColor(edge);
  //         _window.draw(miniChunk);
  //         break;
  //
  //       case NOT_VISIBLE:
  //         miniChunk.setFillColor(fog);
  //         _window.draw(miniChunk);
  //         break;
  //     }
  //   }
  // }
}

void Interface::renderText() const {
  _textTopLeft.render();
  _textTopRight.render();
  _textTopCenter.render();

  if (_textCenterChrono.isStillRunning())
    _textCenter.render();

  if (_textBottomCenterChrono.isStillRunning())
    _textBottomCenter.render();
}

void Interface::setTextTopLeft(const std::string& string) {
  if (!_androidBuild)
    _textTopLeft.setText(string, 12);
}

void Interface::setTextTopRight(const std::string& string) {
  Camera& cam = Camera::getInstance();
  _textTopRight.setText(string, _androidBuild ? 30 : 12);
  _textTopRight.setPosition(cam.getWindowW() - _textTopRight.getSize().x, 0);
}

void Interface::setTextTopCenter(const std::string& string) {
  if (!_androidBuild) {
    Camera& cam = Camera::getInstance();
    _textTopCenter.setText(string, 12);
    _textTopCenter.setPosition(cam.getWindowW() / 2 - _textTopCenter.getSize().x / 2, 0);
  }
}

void Interface::setTextCenter(const std::string& string, int msDuration) {
  if (!_androidBuild) {
    Camera& cam = Camera::getInstance();
    _textCenter.setText(string);
    _textCenter.setPosition(cam.getWindowW() / 2 - _textCenter.getSize().x / 2,
                            cam.getWindowH() / 2 - _textCenter.getSize().y / 2);

    _textCenterChrono.reset(msDuration);
  }
}

void Interface::setTextBottomCenter(const std::string& string, int msDuration) {
  Camera& cam = Camera::getInstance();
  _textBottomCenter.setText(string, _androidBuild ? 38 : 17);
  _textBottomCenter.setPosition(cam.getWindowW() / 2 - _textBottomCenter.getSize().x / 2,
                                cam.getWindowH() - _textBottomCenter.getSize().y);

  _textBottomCenterChrono.reset(msDuration);
}

void Interface::renderRectSelect() const {
  // _window.draw(_rectSelect);
}

void Interface::renderLifeBars(std::set<Lion*> selection) const {
  // sf::RectangleShape lifeBar({20.f, 2.f});
  // lifeBar.setFillColor(sf::Color::Green);
  //
  // sf::RectangleShape fullLifeBar({20.f, 2.f});
  // fullLifeBar.setFillColor(sf::Color::Transparent);
  // fullLifeBar.setOutlineColor(sf::Color::Black);
  // fullLifeBar.setOutlineThickness(1);
  //
  // glm::ivec4 corners;
  // float maxHeightFactor;
  //
  // for(auto it = selection.begin(); it != selection.end(); ++it) {
  //   corners = (*it)->getScreenRect();
  //   // Otherwise the selected element is outside the screen
  //   if (corners.z != 0) {
  //     maxHeightFactor = (*it)->getMaxHeightFactor(); // The lifeBar must not change when switching animations
  //
  //     lifeBar.setSize({20.f* (*it)->getStamina() / 100.f, 2.f});
  //
  //     lifeBar.setPosition(corners.x + corners.z/2 - 10,
  //       corners.y - corners.w*maxHeightFactor + corners.w - 5);
  //     fullLifeBar.setPosition(corners.x + corners.z/2 - 10,
  //       corners.y - corners.w*maxHeightFactor + corners.w - 5);
  //
  //     _window.draw(lifeBar);
  //     _window.draw(fullLifeBar);
  //     lifeBar.setSize({20.f, 2.f});
  //   }
  // }
}

glm::vec2 Interface::getMinimapClickCoord(float x, float y) const {
  // y = _window.getSize().y - y;

  // return glm::vec2( (y - _minimapSprite.getPosition().y) / (float) _minimapSprite.getTextureRect().height,
  //                   (x - _minimapSprite.getPosition().x) / (float) _minimapSprite.getTextureRect().width);
  return glm::vec2(-1,-1);
}

void Interface::setRectSelect(glm::ivec4 rect) {
  // _rectSelect.setPosition(sf::Vector2f(rect.x, rect.y));
  // _rectSelect.setSize    (sf::Vector2f(rect.z, rect.w));
}
