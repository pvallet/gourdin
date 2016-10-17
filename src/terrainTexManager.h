#pragma once

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <string>
#include "utils.h"

class TerrainTexManager {
public:
	TerrainTexManager(std::string folder);

	void bindTexture(Biome biome) const {glBindTexture(GL_TEXTURE_2D, _texIndices[biome]);}
	inline GLuint getTexIndex(Biome biome) const {return _texIndices[biome];}

private:
	GLuint* _texIndices;
};
