#include "interface.h"

#include "camera.h"
#include "coloredRectangles.h"

void Interface::init() {
  ColoredRectangles::loadShader();
  Text::loadShader();
  TexturedRectangle::loadShader();

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
  Camera& cam = Camera::getInstance();

  _minimapTexture.loadFromFile("res/map/map.png");

  _minimapRect.reset(new TexturedRectangle(_minimapTexture.getTexID(), cam.windowRectCoordsToGLRectCoords(glm::uvec4(
    0, cam.getWindowH() - _minimapTexture.getSize().y,
    _minimapTexture.getSize()
  ))));
}

void Interface::renderEngine() const {
  _texRectEngine->draw();
}

void Interface::renderMinimap(const std::vector<std::vector<ChunkStatus> >& chunkStatus) const {
  Camera& cam = Camera::getInstance();
  glm::vec4 minimapTextureRect = _minimapRect->getTextureRect();

  // Background texture
  _minimapRect->draw();

  // Highlight generated chunks
  std::vector<glm::vec4> blackRects;
  std::vector<glm::vec4> darkGreyRects;
  std::vector<glm::vec4> lightGreyRects;

  glm::vec2 miniChunkSize = glm::vec2(minimapTextureRect.z / (float) NB_CHUNKS,
                                      minimapTextureRect.w / (float) NB_CHUNKS);

  glm::vec4 miniChunk(-1, -1, miniChunkSize);

  for (size_t i = 0; i < NB_CHUNKS; i++) {
    miniChunk.y = -1;
    for (size_t j = 0; j < NB_CHUNKS; j++) {
      switch (chunkStatus[i][j]) {
        case NOT_GENERATED:
          blackRects.push_back(miniChunk);
          break;

        case EDGE:
          darkGreyRects.push_back(miniChunk);
          break;

        case NOT_VISIBLE:
          lightGreyRects.push_back(miniChunk);
          break;
      }
      miniChunk.y += miniChunkSize.y;
    }
    miniChunk.x += miniChunkSize.x;
  }

  ColoredRectangles black(glm::vec4(0,0,0,1), blackRects);
  ColoredRectangles darkGrey(glm::vec4(0,0,0,0.8), darkGreyRects);
  ColoredRectangles lightGrey(glm::vec4(0,0,0,0.4), lightGreyRects);
  black.draw();
  darkGrey.draw();
  lightGrey.draw();

  // Position of the viewer

  glm::vec2 viewerPos( minimapTextureRect.x + (float) minimapTextureRect.z * cam.getPointedPos().x / MAX_COORD,
                       minimapTextureRect.y + (float) minimapTextureRect.w * cam.getPointedPos().y / MAX_COORD);

  ColoredRectangles opaqueGrey(glm::vec4(0.2,0.2,0.2,1), std::vector<glm::vec4>(1,
    glm::vec4(viewerPos.x - miniChunkSize.x/2.f,
              viewerPos.y - miniChunkSize.y/2.f,
              miniChunkSize*2.f)
  ));

  opaqueGrey.draw();
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

glm::vec2 Interface::getMinimapClickCoords(size_t x, size_t y) const {
  Camera& cam = Camera::getInstance();

  glm::vec2 clickGLCoords = cam.windowCoordsToGLCoords(glm::uvec2(x,y));
  glm::vec4 minimapTexRect = _minimapRect->getTextureRect();

  glm::vec2 coords = glm::vec2( (clickGLCoords.x - minimapTexRect.x) / minimapTexRect.z,
                                (clickGLCoords.y - minimapTexRect.y) / minimapTexRect.w);

  return coords;
}

void Interface::setRectSelect(glm::ivec4 rect) {
  // _rectSelect.setPosition(sf::Vector2f(rect.x, rect.y));
  // _rectSelect.setSize    (sf::Vector2f(rect.z, rect.w));
}
