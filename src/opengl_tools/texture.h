#pragma once

#include <glm/glm.hpp>

#include "opengl.h"

#include <string>

class Texture {
public:
  Texture ();
  Texture (Texture const&) = delete;
  Texture (Texture&& other) noexcept;
  ~Texture ();

  Texture& operator=(Texture const&) = delete;
  Texture& operator=(Texture&&) = delete;

  void loadFromFile(std::string filePath);

  inline bool isEmpty() const {return _size == glm::uvec2(0,0);}
  inline glm::uvec2 getSize() const {return _size;}

  inline void bind() const {glBindTexture(GL_TEXTURE_2D, _texID);}
  inline void attachToBoundFBO(GLenum attachment) const {
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, _texID, 0);
  }

  static void unbind() {glBindTexture(GL_TEXTURE_2D, 0);}

  // Flip the texture upside down to match OpenGL coordinates system
  static void flipPixelsUpsideDown(size_t width, size_t height, size_t bytesPerPixel,
    unsigned char* data);

private:
  glm::uvec2 _size;

  GLuint _texID;
};
