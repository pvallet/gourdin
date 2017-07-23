#pragma once

#include <string>
#include <vector>

#include "opengl.h"
#include <glm/glm.hpp>

class TextureArray {
public:
	TextureArray() : texID(0), _count(0) {}
	~TextureArray() {glDeleteTextures(1, &texID);}
	TextureArray           (TextureArray const&) = delete;
	TextureArray& operator=(TextureArray const&) = delete;
	TextureArray           (TextureArray&&) = default;
  TextureArray& operator=(TextureArray&&) = default;

	void loadTextures(size_t count, std::string folderPath);

	// Returns the size of the current texture relative to the size of the array texture
	inline glm::vec4 getTexRectangle(size_t index) const {
		return glm::vec4(0,
			               0,
										 texSizes[index].x / maxTexSize.x,
			               texSizes[index].y / maxTexSize.y);
	}

	GLuint texID;
	glm::vec2 maxTexSize;
	std::vector<glm::vec2> texSizes;

private:
	size_t _count;
	std::string _folderPath;
};