#include "texArray.h"

#include <SDL2pp/SDL2pp.hh>
#include <SDL_log.h>

#include <iostream>
#include <sstream>

#define MIPMAP_LVLS 4

TextureArray::TextureArray(TextureArray const &textureArray) {
	loadTextures(textureArray._count, textureArray._folderPath);
}

TextureArray& TextureArray::operator=(TextureArray const &textureArray) {

  loadTextures(textureArray._count, textureArray._folderPath);

  return *this;
}

void TextureArray::loadTextures(size_t count, std::string folderPath) {
	_count = count;
	_folderPath = folderPath;

	if (count != 0) {
		glDeleteTextures(1, &texID);

		std::vector<SDL2pp::Surface> img;
		maxTexSize.x = 0;
		maxTexSize.y = 0;
		texSizes.resize(count);

		for (size_t i = 0; i < count; i++) {
			std::ostringstream convert;
			convert << folderPath << i << ".png";

			img.push_back(SDL2pp::Surface(convert.str()));

			if (img[i].Get()->format->BytesPerPixel == 3) {
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error: image %s has no alpha channel", convert.str().c_str());
				return;
			}

			texSizes[i] = glm::vec2(img[i].GetWidth(), img[i].GetHeight());

			if (texSizes[i].x > maxTexSize.x)
				maxTexSize.x = texSizes[i].x;
			if (texSizes[i].y > maxTexSize.y)
				maxTexSize.y = texSizes[i].y;
		}

		glGenTextures(1, &texID);

		glBindTexture(GL_TEXTURE_2D_ARRAY, texID);

		glTexStorage3D(GL_TEXTURE_2D_ARRAY, MIPMAP_LVLS, GL_RGBA8, maxTexSize.x, maxTexSize.y, count);

		for (size_t i = 0; i < count; i++) {
			glTexSubImage3D( GL_TEXTURE_2D_ARRAY, 0, 0, 0, i,
											 texSizes[i].x, texSizes[i].y, 1,
											 GL_RGBA, GL_UNSIGNED_BYTE, img[i].Get()->pixels
			);
		}

		glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}
}
