#pragma once

#include <glm/glm.hpp>

namespace ut {

glm::vec2 windowCoordsToGLCoords(const glm::ivec2& windowCoords);
glm::ivec2 glCoordsToWindowCoords(const glm::vec2& glCoords);
glm::vec4 windowRectCoordsToGLRectCoords(const glm::ivec4& windowRect);

} // namespace ut
