#include "terrainTexManager.h"

#include <SDL2pp/SDL2pp.hh>
#include <iostream>
#include <sstream>

glm::uvec2 TerrainTexManager::loadTexture(std::string path) {
	SDL2pp::Surface img(path);

  _texIDs.push_back(0);

	glm::uvec2 imgSize(img.GetWidth(), img.GetHeight());

	if (img.Get()->format->BytesPerPixel == 4)
		std::cerr << "Error: image " << path << " has an alpha channel and should not." << '\n';

	glGenTextures(1, &_texIDs.back());

  glBindTexture(GL_TEXTURE_2D, _texIDs.back());

  glTexImage2D(   GL_TEXTURE_2D, 0, GL_RGB,
                  imgSize.x, imgSize.y,
                  0,
                  GL_RGB, GL_UNSIGNED_BYTE, img.Get()->pixels
  );

  glGenerateMipmap(GL_TEXTURE_2D);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glBindTexture(GL_TEXTURE_2D, 0);

	return imgSize;
}

void TerrainTexManager::loadFolder(size_t nbTextures, std::string folderPath) {
  for (size_t i = 0 ; i < nbTextures ; i++) {
    std::ostringstream convert;
    convert << folderPath << i << ".png";
    loadTexture(convert.str());
  }
}
