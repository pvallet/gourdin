#include "terrainTexManager.h"

#include <sstream>

void TerrainTexManager::loadTexture(std::string path) {

	Texture texture;

	texture.loadFromFile(path);

	texture.bind();

  glGenerateMipmap(GL_TEXTURE_2D);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  Texture::unbind();

	_textures.push_back(std::move(texture));
}

void TerrainTexManager::loadFolder(size_t nbTextures, std::string folderPath) {
  for (size_t i = 0 ; i < nbTextures ; i++) {
    std::ostringstream convert;
    convert << folderPath << i << ".png";
    loadTexture(convert.str());
  }
}
