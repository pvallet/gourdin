#include "terrainTexManager.h"

#include <SDL2/SDL_image.h>
#include <iostream>
#include <sstream>

glm::uvec2 TerrainTexManager::loadTexture(std::string path) {
	SDL_Surface* img = nullptr;
  img = IMG_Load(path.c_str());

	if (img) {
	  _texIDs.push_back(0);

		glm::uvec2 imgSize(img->w, img->h);

		if (img->format->BytesPerPixel == 4)
			std::cerr << "Error: image " << path << " has an alpha channel and should not." << '\n';

		glGenTextures(1, &_texIDs.back());

	  glBindTexture(GL_TEXTURE_2D, _texIDs.back());

	  glTexImage2D(   GL_TEXTURE_2D, 0, GL_RGB,
	                  imgSize.x, imgSize.y,
	                  0,
	                  GL_RGB, GL_UNSIGNED_BYTE, img->pixels
	  );

	  glGenerateMipmap(GL_TEXTURE_2D);

	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	  glBindTexture(GL_TEXTURE_2D, 0);

		SDL_FreeSurface(img);

		return imgSize;
	}

	else {
		std::cerr << "Unable to load texture: " << path << ", " << SDL_GetError() << std::endl;
		return glm::uvec2(0,0);
	}
}

void TerrainTexManager::loadFolder(size_t nbTextures, std::string folderPath) {
  for (size_t i = 0 ; i < nbTextures ; i++) {
    std::ostringstream convert;
    convert << folderPath << i << ".png";
    loadTexture(convert.str());
  }
}
