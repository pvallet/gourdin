#pragma once

#include <GL/glew.h>
#include "opengl.h"

#include <map>
#include <vector>

#include "terrainGeometry.h"
#include "terrainTexManager.h"
#include "igElementDisplay.h"
#include "utils.h"

struct BiomeIndices {
	std::vector<GLuint> indices;
	GLuint ibo;
};

class Buffers {
public:
	Buffers() : vao(0), vbo(0) {}

	GLuint vao;
	GLuint vbo;

	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<float> coords;

	glm::vec3 corners[8]; // For frustum culling

	std::map<Biome, BiomeIndices> indicesInfo;
};

class Chunk {
public:
	Chunk(size_t x, size_t y, const TerrainTexManager& terrainTexManager,
		                              TerrainGeometry& terrainGeometry);
	~Chunk();
	Chunk(Chunk const&)          = delete;
	void operator=(Chunk const&) = delete;

	void generate();
	size_t draw() const;

	// Set visible to false if there is no need to display the chunk
	void computeCulling();
	void computeSubdivisionLevel();
	void setSubdivisionLevel(size_t newSubdLvl);

	float getHeight(glm::vec2 pos) const;
	glm::vec3 getNorm(glm::vec2 pos) const;
	inline bool isVisible() const {return _visible;}
	size_t getSubdivisionLevel() const {return _currentSubdivLvl;}

	void setTrees(std::vector<igElement*> trees);
	inline size_t drawTrees() const {_treeDrawer.drawElements(); return _trees.size();}

private:
	void cleanSubdivLvl(size_t subdivLvl);
	GLuint addVertexInfo(Vertex* vertex);
	void fillBufferData();
	void generateBuffers();
	void computeChunkBoundingBox();
	bool theCornersAreOutside(glm::vec3 cam, glm::vec3 vec) const;
	void setTreesHeights();

	glm::ivec2 _chunkPos;

	bool _visible;
	size_t _currentSubdivLvl;
	std::vector<std::unique_ptr<Buffers> > _subdivisionLevels;

	const TerrainTexManager& _terrainTexManager;
	TerrainGeometry&   _terrainGeometry;

	std::vector<igElement*> _trees;
	igElementDisplay _treeDrawer;
};
