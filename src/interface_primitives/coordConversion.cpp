#include "coordConversion.h"

#include "camera.h"

glm::vec2 ut::windowCoordsToGLCoords(const glm::ivec2& windowCoords) {
  Camera& cam = Camera::getInstance();
  return glm::vec2(    2 * windowCoords.x / (float) cam.getWindowW() - 1,
                   1 - 2 * windowCoords.y / (float) cam.getWindowH());
}

glm::ivec2 ut::glCoordsToWindowCoords(const glm::vec2& glCoords) {
  Camera& cam = Camera::getInstance();
  return glm::ivec2(round(cam.getWindowW() * (1 + glCoords.x) / 2),
                    round(cam.getWindowH() * (1 - glCoords.y) / 2));
}

glm::vec4 ut::windowRectCoordsToGLRectCoords(const glm::ivec4& windowRect) {
  Camera& cam = Camera::getInstance();
  return glm::vec4(2 *  windowRect.x / (float) cam.getWindowW() - 1,
                   1 - 2 * (windowRect.y + windowRect.w) / (float) cam.getWindowH(),
                   2 *  windowRect.z / (float) cam.getWindowW(),
                   2 *  windowRect.w / (float) cam.getWindowH());
}
