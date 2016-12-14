#pragma once

#include <string>
#include <vector>

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>

class TextureArray {
public:
	GLuint texID;
	sf::Vector2f maxTexSize;
	std::vector<sf::Vector2f> texSizes;

	inline sf::FloatRect getTexRectangle(size_t index) const {
		return sf::FloatRect(0,
			                   0,
			                   texSizes[index].x / maxTexSize.x,
			                   texSizes[index].y / maxTexSize.y);
	}
};

class TexArrayManager {
public:
	TexArrayManager() {}
	~TexArrayManager();

  const TextureArray* loadTextures(size_t count, std::string folderPath);

private:
	std::vector<std::unique_ptr<TextureArray> > _texArrays;
};
