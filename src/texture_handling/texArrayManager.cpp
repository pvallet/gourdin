#include "texArrayManager.h"
#include <iostream>
#include <sstream>

#include <SFML/Graphics.hpp>

#define MIPMAP_LVLS 4

TexArrayManager::~TexArrayManager() {
  for (size_t i = 0; i < _texArrays.size(); i++) {
    glDeleteTextures(1, &_texArrays[i]->texID);
  }
}

const TextureArray* TexArrayManager::loadTextures(size_t count, std::string folderPath) {
	std::vector<sf::Image> img(count);
  TextureArray ta;
  ta.maxTexSize.x = 0;
  ta.maxTexSize.y = 0;
  ta.texSizes.resize(count);

  for (size_t i = 0; i < count; i++) {
    std::ostringstream convert;
    convert << folderPath << i << ".png";

    if (!img[i].loadFromFile(convert.str())) {
      std::cerr << "Unable to open file " << std::endl;
    }

    ta.texSizes[i] = sf::Vector2f(img[i].getSize().x, img[i].getSize().y);

    if (ta.texSizes[i].x > ta.maxTexSize.x)
      ta.maxTexSize.x = ta.texSizes[i].x;
    if (ta.texSizes[i].y > ta.maxTexSize.y)
      ta.maxTexSize.y = ta.texSizes[i].y;
  }

  ta.texID = 0;
  glGenTextures(1, &ta.texID);

  glBindTexture(GL_TEXTURE_2D_ARRAY, ta.texID);

  glTexStorage3D(GL_TEXTURE_2D_ARRAY, MIPMAP_LVLS, GL_RGBA8, ta.maxTexSize.x, ta.maxTexSize.y, count);

  for (size_t i = 0; i < count; i++) {
    glTexSubImage3D( GL_TEXTURE_2D_ARRAY, 0, 0, 0, i,
                     ta.texSizes[i].x, ta.texSizes[i].y, 1,
                     GL_RGBA, GL_UNSIGNED_BYTE, img[i].getPixelsPtr()
    );
  }

  glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glBindTexture(GL_TEXTURE_2D, 0);

  _texArrays.push_back(std::unique_ptr<TextureArray>(new TextureArray(ta)));

  return _texArrays.back().get();
}
