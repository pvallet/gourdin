#include "interface.h"

Interface::Interface():
  cam(Camera::getInstance()),
  _rectSelect(glm::vec4(1), false) {}

void Interface::init() {
  ColoredRectangles::loadShader();
  Text::loadShader();
  TexturedRectangle::loadShader();

#ifdef __ANDROID__
  _androidBuild = true;
#else
  _androidBuild = false;
#endif

  // Minimap
  _minimapTexture.loadFromFile("res/map/map.png");

#ifndef __ANDROID__
  _minimapRect.reset(new TexturedRectangle(&_minimapTexture, cam.windowRectCoordsToGLRectCoords(glm::uvec4(
    0, cam.getWindowH() - _minimapTexture.getSize().y,
    _minimapTexture.getSize()
  ))));
#else
  _minimapRect.reset(new TexturedRectangle(&_minimapTexture, cam.windowRectCoordsToGLRectCoords(glm::uvec4(
    cam.getWindowW() - _minimapTexture.getSize().x * cam.getWindowW()/cam.getW(),
    cam.getWindowH() - _minimapTexture.getSize().y * cam.getWindowH()/cam.getH(),
    _minimapTexture.getSize().x * cam.getWindowW()/cam.getW(),
    _minimapTexture.getSize().y * cam.getWindowH()/cam.getH()
  ))));
#endif
}

void Interface::renderEngine() const {
  _texRectEngine->draw();
}

void Interface::renderMinimap(const Engine& engine) const {
  glm::vec4 minimapTextureRect = _minimapRect->getTextureRect();

  // Background texture
  _minimapRect->draw();

  // Highlight visible chunks
  std::vector<glm::vec4> greyRects;

  glm::vec2 miniChunkSize = glm::vec2(minimapTextureRect.z / (float) NB_CHUNKS,
                                      minimapTextureRect.w / (float) NB_CHUNKS);

  glm::vec4 miniChunk(minimapTextureRect.x, minimapTextureRect.y, miniChunkSize);

  for (size_t i = 0; i < NB_CHUNKS; i++) {
    miniChunk.y = minimapTextureRect.y;
    for (size_t j = 0; j < NB_CHUNKS; j++) {

      if (!engine.isChunkVisible(i,j))
        greyRects.push_back(miniChunk);

      miniChunk.y += miniChunkSize.y;
    }
    miniChunk.x += miniChunkSize.x;
  }

  ColoredRectangles grey(glm::vec4(0,0,0,0.4), greyRects);
  grey.draw();

  // Position of the viewer

  glm::vec2 viewerPos( minimapTextureRect.x + (float) minimapTextureRect.z * cam.getPointedPos().x / MAX_COORD,
                       minimapTextureRect.y + (float) minimapTextureRect.w * cam.getPointedPos().y / MAX_COORD);

  ColoredRectangles opaqueGrey(glm::vec4(0.2,0.2,0.2,1), std::vector<glm::vec4>(1,
    glm::vec4(viewerPos.x - miniChunkSize.x/2.f,
              viewerPos.y - miniChunkSize.y/2.f,
              miniChunkSize*2.f)
  ));

  opaqueGrey.draw();

  // Texture frame
  ColoredRectangles frame(glm::vec4(0.52, 0.34, 0.138, 1), minimapTextureRect, false);
  frame.draw();
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
  _textTopRight.setText(string, _androidBuild ? 30 : 12);
  _textTopRight.setPosition(cam.getWindowW() - _textTopRight.getSize().x, 0);
}

void Interface::setTextTopCenter(const std::string& string) {
  if (!_androidBuild) {
    _textTopCenter.setText(string, 12);
    _textTopCenter.setPosition(cam.getWindowW() / 2 - _textTopCenter.getSize().x / 2, 0);
  }
}

void Interface::setTextCenter(const std::string& string, int msDuration) {
  if (!_androidBuild) {
    _textCenter.setText(string);
    _textCenter.setPosition(cam.getWindowW() / 2 - _textCenter.getSize().x / 2,
                            cam.getWindowH() / 2 - _textCenter.getSize().y / 2);

    _textCenterChrono.reset(msDuration);
  }
}

void Interface::setTextBottomCenter(const std::string& string, int msDuration) {
  _textBottomCenter.setText(string, _androidBuild ? 38 : 17);
  _textBottomCenter.setPosition(cam.getWindowW() / 2 - _textBottomCenter.getSize().x / 2,
                                cam.getWindowH() - _textBottomCenter.getSize().y);

  _textBottomCenterChrono.reset(msDuration);
}

void Interface::renderRectSelect() const {
  _rectSelect.draw();
}

void Interface::renderLifeBars(std::set<Lion*> selection) const {
  std::vector<glm::vec4> staminaBarsRects;
  std::vector<glm::vec4> outlinesRects;

  for(auto it = selection.begin(); it != selection.end(); ++it) {
    glm::uvec4 corners = (*it)->getScreenRect();

    // Otherwise the selected element is outside the screen
    if (corners.z != 0) {
      float maxHeightFactor = (*it)->getMaxHeightFactor(); // The lifeBar must not change when switching animations

      staminaBarsRects.push_back(cam.windowRectCoordsToGLRectCoords(glm::uvec4(
        corners.x + corners.z/2 - 10,
        corners.y - corners.w*maxHeightFactor + corners.w - 5,
        20.f* (*it)->getStamina() / 100.f,
        4
      )));

      outlinesRects.push_back(cam.windowRectCoordsToGLRectCoords(glm::uvec4(
        corners.x + corners.z/2 - 10,
        corners.y - corners.w*maxHeightFactor + corners.w - 5,
        20,
        4
      )));
    }
  }

  ColoredRectangles staminaBars(glm::vec4(0,1,0,1), staminaBarsRects);
  ColoredRectangles outlines(glm::vec4(0,0,0,1), outlinesRects, false);
  staminaBars.draw();
  outlines.draw();
}

glm::vec2 Interface::getMinimapClickCoords(size_t x, size_t y) const {
  glm::vec2 clickGLCoords = cam.windowCoordsToGLCoords(glm::uvec2(x,y));
  glm::vec4 minimapTexRect = _minimapRect->getTextureRect();

  glm::vec2 coords = glm::vec2( (clickGLCoords.x - minimapTexRect.x) / minimapTexRect.z,
                                (clickGLCoords.y - minimapTexRect.y) / minimapTexRect.w);

  return coords;
}

void Interface::setRectSelect(glm::ivec4 rect) {
  glm::uvec4 absoluteRect;
  if (rect.z > 0) {
    absoluteRect.x = rect.x;
    absoluteRect.z = rect.z;
  }
  else {
    absoluteRect.x = rect.x + rect.z;
    absoluteRect.z = - rect.z;
  }

  if (rect.w > 0) {
    absoluteRect.y = rect.y;
    absoluteRect.w = rect.w;
  }
  else {
    absoluteRect.y = rect.y + rect.w;
    absoluteRect.w = - rect.w;
  }

  _rectSelect.setRectangles(cam.windowRectCoordsToGLRectCoords(absoluteRect));
}
