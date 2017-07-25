#include "texture.h"

#include <SDL2pp/SDL2pp.hh>
#include <SDL_log.h>

Texture::Texture ():
  _texID(0) {
  glGenTextures(1, &_texID);
}

Texture::~Texture() {
  glDeleteTextures(1, &_texID);
}

Texture::Texture(Texture&& other) noexcept :
  _size(other._size),
  _texID(other._texID) {

  other._texID = 0;
}

void Texture::loadFromFile(std::string filePath) {
  SDL2pp::Surface img(filePath);

  // Will be same as internalFormat
  GLenum format(0);

  if (img.Get()->format->BytesPerPixel == 3)
    format = GL_RGB;

  else if (img.Get()->format->BytesPerPixel == 4)
    format = GL_RGBA;

  else
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error in Texture::loadFromFile: format is not true color");

  // Opengl ES does not handle GL_BGR images
  if(img.Get()->format->Rmask != 0xff)
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error in Texture::loadFromFile: format not handled");

  _size.x = img.GetWidth();
  _size.y = img.GetHeight();

  flipPixelsUpsideDown(_size.x, _size.y, img.Get()->format->BytesPerPixel, (unsigned char*) img.Get()->pixels);

  bind();

  glTexImage2D(GL_TEXTURE_2D, 0, format, _size.x, _size.y, 0, format, GL_UNSIGNED_BYTE, img.Get()->pixels);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  unbind();
}

void Texture::flipPixelsUpsideDown(size_t width, size_t height, size_t bytesPerPixel,
  unsigned char* data) {

  for (size_t i = 0; i < width / 2; i++) {
    for (size_t j = 0; j < height; j++) {
      for (size_t k = 0; k < bytesPerPixel; k++) {
        std::swap(data[bytesPerPixel * (i * width + j) + k],
                  data[bytesPerPixel * (width * (height - i - 1) + j) + k]);
      }
    }
  }
}
