#pragma once

#include <glm/glm.hpp>

#include "opengl.h"
#include "texture.h"

#include <string>
#include <vector>

class TerrainTexManager {
public:
	TerrainTexManager() {}

	void loadTexture(std::string path);
	void loadFolder(size_t nbTextures, std::string folderPath);

	void bindTexture(size_t index) const {_textures[index].bind();}

	inline const Texture* getTexture(size_t index) const {return &_textures[index];}

private:
	std::vector<Texture> _textures;
};
