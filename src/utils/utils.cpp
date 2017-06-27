#include "utils.h"

#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

#include "camera.h"

#include <iostream>
#include <string>

glm::uvec2 ut::convertToChunkCoords(glm::vec2 pos) {
  glm::uvec2 chunkPos;
  if (pos.x < 0)
    pos.x = 0;
  if (pos.y < 0)
    pos.y = 0;

  chunkPos.x = pos.x / CHUNK_SIZE;
  chunkPos.y = pos.y / CHUNK_SIZE;

  if (chunkPos.x >= NB_CHUNKS)
    chunkPos.x = NB_CHUNKS-1;
  if (chunkPos.y >= NB_CHUNKS)
    chunkPos.y = NB_CHUNKS-1;

  return chunkPos;
}

glm::vec3 ut::carthesian(float r, float theta, float phi) {
  glm::vec3 u;

  u.x = r*sin(phi*RAD)*cos(theta*RAD);
  u.y = r*sin(phi*RAD)*sin(theta*RAD);
  u.z = r*cos(phi*RAD);

  return u;
}

glm::vec3 ut::spherical(float x, float y, float z) {
	glm::vec3 u(0,0,0);
	u.x = sqrt(x*x + y*y + z*z);
	if (u.x != 0) {
		u.y = atan2(y,x) / RAD;
		if (u.y < 0)
			u.y += 360;

		u.z = acos(z/u.x) / RAD;
	}

	return u;
}

bool glCheckError(const char *file, int line) {
  GLenum err (glGetError());

  bool isError = false;

  while(err != GL_NO_ERROR) {
    std::string error;

    switch(err) {
      case GL_INVALID_OPERATION:             error="INVALID_OPERATION";      break;
      case GL_INVALID_ENUM:                  error="INVALID_ENUM";           break;
      case GL_INVALID_VALUE:                 error="INVALID_VALUE";          break;
      case GL_OUT_OF_MEMORY:                 error="OUT_OF_MEMORY";          break;
      case GL_INVALID_FRAMEBUFFER_OPERATION: error="INVALID_FRAMEBUFFER_OPERATION";  break;
      case GL_STACK_UNDERFLOW:               error="STACK_UNDERFLOW";        break;
      case GL_STACK_OVERFLOW:                error="STACK_OVERFLOW";         break;
    }

    std::cerr << "GL_" << error << " - " << file << ":" << line << std::endl;
    err = glGetError();
    isError = true;
  }

  return isError;
}

void LogText::addFPSandCamInfo(int msElapsed) {
  Camera& cam = Camera::getInstance();
  int fps = 0;

  if (msElapsed != 0)
    fps = 1.f / msElapsed * 1000;

  _text << "X: " << cam.getPointedPos().x << "\n"
  << "Y: " << cam.getPointedPos().y << std::endl;
  _text << "R: " << cam.getZoom() << "\n"
  << "Theta: " << cam.getTheta() - 360 * (int) (cam.getTheta() / 360) +
  (cam.getTheta() < 0 ? 360 : 0) << "\n"
  << "Phi: " << cam.getPhi() << std::endl;
  _text << "FPS: " << fps << std::endl;
}
