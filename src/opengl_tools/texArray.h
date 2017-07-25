#pragma once

#include <string>
#include <vector>

#include "opengl.h"
#include <glm/glm.hpp>

class TextureArray {
public:
	TextureArray();
	TextureArray(TextureArray const&) = delete;
	TextureArray(TextureArray&& other) noexcept;
	~TextureArray();

	TextureArray& operator=(TextureArray const&) = delete;
  TextureArray& operator=(TextureArray&&) = delete;

	void loadTextures(size_t count, std::string folderPath);

	// Returns the size of the current texture relative to the size of the array texture
	inline glm::vec4 getTexRectangle(size_t index) const {
		return glm::vec4(0,
			               0,
										 _texSizes[index].x / _maxTexSize.x,
			               _texSizes[index].y / _maxTexSize.y);
	}

	inline const glm::vec2& getMaxTexSize() const {return _maxTexSize;}
	inline const std::vector<glm::vec2>& getTexSizes() const {return _texSizes;}

	inline void bind() const {glBindTexture(GL_TEXTURE_2D_ARRAY, _texID);}

	static void unbind() {glBindTexture(GL_TEXTURE_2D_ARRAY, 0);}

private:
	GLuint _texID;
	size_t _count;
	std::string _folderPath;

	glm::vec2 _maxTexSize;
	std::vector<glm::vec2> _texSizes;
};
