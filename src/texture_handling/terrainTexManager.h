#pragma once

#include <string>
#include <vector>

#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <glm/glm.hpp>

class TerrainTexManager {
public:
	TerrainTexManager() {}
  ~TerrainTexManager() {glDeleteTextures(_texIDs.size(), &_texIDs[0]);}
	TerrainTexManager(TerrainTexManager const&) = delete;
	void operator=   (TerrainTexManager const&) = delete;

	glm::uvec2 loadTexture(std::string path);
	void loadFolder(size_t nbTextures, std::string folderPath);

	void bindTexture(size_t index) const {glBindTexture(GL_TEXTURE_2D, _texIDs[index]);}

	inline GLuint getTexID(size_t index) const {return _texIDs[index];}

private:
	std::vector<GLuint> _texIDs;
};
