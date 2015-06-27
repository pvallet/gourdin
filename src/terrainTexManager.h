#pragma once

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <string>
#include "utils.h"

class TerrainTexManager {
public:
	TerrainTexManager(std::string folder);

	void bindTexture(Biome biome) const {glBindTexture(GL_TEXTURE_2D, texIndices[biome]);}
	inline GLuint getTexIndex(Biome biome) const {return texIndices[biome];}

private:
	GLuint* texIndices;
};
