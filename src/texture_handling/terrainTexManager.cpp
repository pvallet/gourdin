#include "terrainTexManager.h"
#include <iostream>
#include <sstream>

#include <SFML/Graphics.hpp>

sf::Vector2u TerrainTexManager::loadTexture(std::string folder) {
	sf::Image img;

  if (!img.loadFromFile(folder)) {
    std::cerr << "Unable to open file " << std::endl;
  }

  _texIDs.push_back(0);

	glGenTextures(1, &_texIDs.back());

  glBindTexture(GL_TEXTURE_2D, _texIDs.back());

  glTexImage2D(   GL_TEXTURE_2D, 0, GL_RGBA,
                  img.getSize().x, img.getSize().y,
                  0,
                  GL_RGBA, GL_UNSIGNED_BYTE, img.getPixelsPtr()
  );

  glGenerateMipmap(GL_TEXTURE_2D);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glBindTexture(GL_TEXTURE_2D, 0);

	return img.getSize();
}

void TerrainTexManager::loadFolder(size_t nbTextures, std::string folderPath) {
  for (size_t i = 0 ; i < nbTextures ; i++) {
    std::ostringstream convert;
    convert << folderPath << i << ".png";
    loadTexture(convert.str());
  }
}
