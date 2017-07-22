#pragma once

#include <glm/glm.hpp>

#include "opengl.h"

#include <string>

class Texture {
public:
  Texture ();
  ~Texture ();
  Texture        (Texture const&) = delete;
  void operator= (Texture const&) = delete;

  void loadFromFile(std::string filePath);

  inline bool isEmpty() const {return _size == glm::uvec2(0,0);}
  inline glm::uvec2 getSize() const {return _size;}
  inline GLuint getTexID() const {return _texID;}

  // Flip the texture upside down to match OpenGL coordinates system
  static void flipPixelsUpsideDown(size_t width, size_t height, size_t bytesPerPixel,
    unsigned char* data);

private:
  void deleteTexture();

  glm::uvec2 _size;

  GLuint _texID;
};
