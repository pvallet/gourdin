#include "texArray.h"

#include <SDL2pp/SDL2pp.hh>
#include <SDL_log.h>

#include <sstream>

#define MIPMAP_LVLS 4

TextureArray::TextureArray(): _texID(0), _count(0) {
	glGenTextures(1, &_texID);
}

TextureArray::~TextureArray() {
	glDeleteTextures(1, &_texID);
}

TextureArray::TextureArray(TextureArray&& other) noexcept:
 	_texID(other._texID),
	_count(other._texID) {
	other._texID = 0;
}

void TextureArray::loadTextures(size_t count, std::string folderPath) {
	_count = count;
	_folderPath = folderPath;

	if (count != 0) {
		std::vector<SDL2pp::Surface> img;
		_maxTexSize.x = 0;
		_maxTexSize.y = 0;
		_texSizes.resize(count);

		for (int i = 0; i < count; i++) {
			std::ostringstream convert;
			convert << folderPath << i << ".png";

			img.push_back(SDL2pp::Surface(convert.str()));

			if (img[i].Get()->format->BytesPerPixel == 3) {
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error: image %s has no alpha channel", convert.str().c_str());
				return;
			}

			_texSizes[i] = glm::vec2(img[i].GetWidth(), img[i].GetHeight());

			if (_texSizes[i].x > _maxTexSize.x)
				_maxTexSize.x = _texSizes[i].x;
			if (_texSizes[i].y > _maxTexSize.y)
				_maxTexSize.y = _texSizes[i].y;
		}

		bind();

		glTexStorage3D(GL_TEXTURE_2D_ARRAY, MIPMAP_LVLS, GL_RGBA8, _maxTexSize.x, _maxTexSize.y, count);

		for (int i = 0; i < count; i++) {
			glTexSubImage3D( GL_TEXTURE_2D_ARRAY, 0, 0, 0, i,
											 _texSizes[i].x, _texSizes[i].y, 1,
											 GL_RGBA, GL_UNSIGNED_BYTE, img[i].Get()->pixels
			);
		}

		glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		unbind();
	}
}
