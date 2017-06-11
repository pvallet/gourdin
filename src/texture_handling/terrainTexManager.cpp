#include "terrainTexManager.h"

#include <SFML/Graphics.hpp>
#include <sstream>

glm::uvec2 TerrainTexManager::loadTexture(std::string folder) {
	sf::Image img;

  img.loadFromFile(folder);

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

	return glm::uvec2(img.getSize().x, img.getSize().y);
}

void TerrainTexManager::loadFolder(size_t nbTextures, std::string folderPath) {
  for (size_t i = 0 ; i < nbTextures ; i++) {
    std::ostringstream convert;
    convert << folderPath << i << ".png";
    loadTexture(convert.str());
  }
}
