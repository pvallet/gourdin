#include "igLayout.h"

igLayout::igLayout():
  cam(Camera::getInstance()),
  interfaceParams(InterfaceParameters::getInstance()),
  _rectSelect(glm::vec4(1), false) {}

void igLayout::init() {
  // Minimap
  _minimapTexture.loadFromFile("res/map/map.png");

#ifndef __ANDROID__
  _minimapRect.reset(new TexturedRectangle(&_minimapTexture, cam.windowRectCoordsToGLRectCoords(glm::uvec4(
    0, cam.getWindowH() - _minimapTexture.getSize().y,
    _minimapTexture.getSize()
  ))));
#else
  _minimapRect.reset(new TexturedRectangle(&_minimapTexture, cam.windowRectCoordsToGLRectCoords(glm::uvec4(
    cam.getWindowW() - _minimapTexture.getSize().x * 2 * interfaceParams.getAndroidInterfaceZoomFactor(),
    cam.getWindowH() - _minimapTexture.getSize().y * 2 * interfaceParams.getAndroidInterfaceZoomFactor(),
    _minimapTexture.getSize().x * 2 * interfaceParams.getAndroidInterfaceZoomFactor(),
    _minimapTexture.getSize().y * 2 * interfaceParams.getAndroidInterfaceZoomFactor()
  ))));
#endif
}

void igLayout::renderEngine() const {
  _texRectEngine->draw();
}

void igLayout::renderMinimap(const Engine& engine) const {
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

  glm::vec2 viewer = glm::vec2(minimapTextureRect.z, minimapTextureRect.w) / 10.f;

  ColoredRectangles opaqueGrey(glm::vec4(0.2,0.2,0.2,1), std::vector<glm::vec4>(1,
    glm::vec4(viewerPos.x - viewer.x/2.f,
              viewerPos.y - viewer.y/2.f,
              viewer)
  ));

  opaqueGrey.draw();

  // Texture frame
  ColoredRectangles frame(interfaceParams.colorFrame(), minimapTextureRect, false);
  frame.draw();
}

void igLayout::renderText() const {
  _textTopLeft.draw();
  _textTopRight.draw();
  _textTopCenter.draw();

  if (_textCenterChrono.isStillRunning())
    _textCenter.draw();

  if (_textBottomCenterChrono.isStillRunning())
    _textBottomCenter.draw();
}

void igLayout::setTextTopLeft(const std::string& string) {
  _textTopLeft.setText(string, interfaceParams.sizeTextSmall());
}

void igLayout::setTextTopRight(const std::string& string) {
  _textTopRight.setText(string, interfaceParams.sizeTextSmall());
  _textTopRight.setPosition(cam.getWindowW() - _textTopRight.getSize().x, 0);
}

void igLayout::setTextTopCenter(const std::string& string) {
    _textTopCenter.setText(string, interfaceParams.sizeTextSmall());
    _textTopCenter.setPosition(cam.getWindowW() / 2 - _textTopCenter.getSize().x / 2, 0);
}

void igLayout::setTextCenter(const std::string& string, int msDuration) {
  _textCenter.setText(string, interfaceParams.sizeTextBig());
  _textCenter.setPosition(cam.getWindowW() / 2 - _textCenter.getSize().x / 2,
                          cam.getWindowH() / 2 - _textCenter.getSize().y / 2);

  _textCenterChrono.reset(msDuration);
}

void igLayout::setTextBottomCenter(const std::string& string, int msDuration) {
  _textBottomCenter.setText(string, interfaceParams.sizeTextMedium());
  _textBottomCenter.setPosition(cam.getWindowW() / 2 - _textBottomCenter.getSize().x / 2,
                                cam.getWindowH() - _textBottomCenter.getSize().y);

  _textBottomCenterChrono.reset(msDuration);
}

void igLayout::renderRectSelect() const {
  _rectSelect.draw();
}

void igLayout::renderStaminaBars(std::set<Lion*> selection) const {
  std::vector<glm::vec4> staminaBarsRects;
  std::vector<glm::vec4> outlinesRects;

  for(auto it = selection.begin(); it != selection.end(); ++it) {
    glm::uvec4 corners = (*it)->getScreenRect();

    // Otherwise the selected element is outside the screen
    if (corners.z != 0) {
      float maxHeightFactor = (*it)->getMaxHeightFactor(); // The lifeBar must not change when switching animations

      staminaBarsRects.push_back(cam.windowRectCoordsToGLRectCoords(glm::uvec4(
        corners.x + corners.z/2 - interfaceParams.staminaBarWidth() / 2.f,
        corners.y - corners.w*maxHeightFactor + corners.w - interfaceParams.staminaBarWidth() / 4.f,
        interfaceParams.staminaBarWidth() * (*it)->getStamina() / 100.f,
        interfaceParams.staminaBarHeight()
      )));

      outlinesRects.push_back(cam.windowRectCoordsToGLRectCoords(glm::uvec4(
        corners.x + corners.z/2 - interfaceParams.staminaBarWidth() / 2.f,
        corners.y - corners.w*maxHeightFactor + corners.w - interfaceParams.staminaBarWidth() / 4.f,
        interfaceParams.staminaBarWidth(),
        interfaceParams.staminaBarHeight()
      )));
    }
  }

  ColoredRectangles staminaBars(glm::vec4(0,1,0,1), staminaBarsRects);
  ColoredRectangles outlines(glm::vec4(0,0,0,1), outlinesRects, false);
  staminaBars.draw();
  outlines.draw();
}

glm::vec2 igLayout::getMinimapClickCoords(size_t x, size_t y) const {
  glm::vec2 clickGLCoords = cam.windowCoordsToGLCoords(glm::uvec2(x,y));
  glm::vec4 minimapTexRect = _minimapRect->getTextureRect();

  glm::vec2 coords = glm::vec2( (clickGLCoords.x - minimapTexRect.x) / minimapTexRect.z,
                                (clickGLCoords.y - minimapTexRect.y) / minimapTexRect.w);

  return coords;
}

void igLayout::setRectSelect(glm::ivec4 rect) {
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
