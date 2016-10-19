#pragma once

#include <string>
#include <vector>

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/System.hpp>

class TexManager {
public:
	TerrainTexManager() {}

	sf::Vector2u loadTexture(std::string path);
	void loadFolder(size_t nbTextures, std::string folderPath);

	void bindTexture(size_t index) const {glBindTexture(GL_TEXTURE_2D, _texIDs[index]);}

	inline GLuint getTexID(size_t index) const {return _texIDs[index];}

private:
	std::vector<GLuint> _texIDs;
};
