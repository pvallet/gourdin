#include "texture.h"

#include <SDL2pp/SDL2pp.hh>
#include <SDL_log.h>

Texture::Texture ():
  _texID(0) {}

Texture::~Texture() {
  deleteTexture();
}

void Texture::deleteTexture() {
  if(glIsTexture(_texID) == GL_TRUE)
    glDeleteTextures(1, &_texID);
}

void Texture::loadFromFile(std::string filePath) {
  deleteTexture();

  SDL2pp::Surface img(filePath);

  GLenum internalFormat(0);
  GLenum format(0);

  if (img.Get()->format->BytesPerPixel == 3) {
    internalFormat = GL_RGB;

    if(img.Get()->format->Rmask == 0xff)
      format = GL_RGB;
    else
      format = GL_BGR;
  }

  else if (img.Get()->format->BytesPerPixel == 4) {
    internalFormat = GL_RGBA;

    if(img.Get()->format->Rmask == 0xff)
      format = GL_RGBA;

    else
      format = GL_BGRA;
  }

  else
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error in Texture::loadFromFile: format is not true color");

  _size.x = img.GetWidth();
  _size.y = img.GetHeight();

  flipPixelsUpsideDown(_size.x, _size.y, img.Get()->format->BytesPerPixel, (unsigned char*) img.Get()->pixels);

  glGenTextures(1, &_texID);
  glBindTexture(GL_TEXTURE_2D, _texID);

  glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, _size.x, _size.y, 0, format, GL_UNSIGNED_BYTE, img.Get()->pixels);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::flipPixelsUpsideDown(size_t width, size_t height, size_t bytesPerPixel,
  unsigned char* data) {

  for (size_t i = 0; i < width / 2; i++) {
    for (size_t j = 0; j < height; j++) {
      for (size_t k = 0; k < bytesPerPixel; k++) {
        std::swap(data[bytesPerPixel * (i * width + j) + k],
                  data[bytesPerPixel * (width * (width - i - 1) + j) + k]);
      }
    }
  }
}
