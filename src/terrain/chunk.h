#pragma once

#include <map>
#include <vector>

#include "basicGLObjects.h"
#include "terrainGeometry.h"
#include "terrainTexManager.h"
#include "igElementDisplay.h"

struct BiomeIndices {
	std::vector<GLuint> indices;
	IndexBufferObject ibo;
};

struct Buffers {
	bool generated = false;
	VertexArrayObject vao;
	VertexBufferObject vbo;

	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<float> coords;

	glm::vec3 corners[8]; // For frustum culling

	igElementDisplay treeDrawer;

	std::map<Biome, BiomeIndices> indicesInfo;
};

class Chunk {
public:
	Chunk(size_t x, size_t y, const TerrainTexManager& terrainTexManager,
		                              TerrainGeometry& terrainGeometry);

	void generateSubdivisionLevel(size_t level);
	size_t draw() const;

	// Set visible to false if there is no need to display the chunk
	void computeCulling(const std::vector<glm::vec3>& planeNormals);
	void computeDistanceOptimizations();
	void setSubdivisionLevel(size_t newSubdLvl);

	float getHeight(glm::vec2 pos) const;
	glm::vec3 getNorm(glm::vec2 pos) const;
	inline bool isVisible() const {return _visible;}
	inline bool getTreesNeedTwoPasses() const {return _treesNeedTwoPasses;}
	inline bool getDisplayMovingElements() const {return _displayMovingElements;}
	size_t getSubdivisionLevel() const {return _currentSubdivLvl;}

	inline void setTrees(std::vector<igElement*> trees) {_trees = trees;}
	inline size_t drawTrees() const {_subdivisionLevels[_currentSubdivLvl]->treeDrawer.drawElements(); return _trees.size();}

private:
	void cleanSubdivLvl(size_t subdivLvl);
	void fillBufferData(size_t subdivLvl);
	void generateBuffers();
	void computeChunkBoundingBox(size_t subdivLvl);
	bool theCornersAreOutside(const glm::vec3& cam, const glm::vec3& vec) const;

	float getHeight(glm::vec2 pos, size_t subdivLvl) const;

	glm::ivec2 _chunkPos;
	glm::vec3 _centerOfChunk;

	bool _visible;
	bool _treesNeedTwoPasses;
	bool _displayMovingElements;
	size_t _currentSubdivLvl;
	size_t _maxSubdivLvl;
	std::vector<std::unique_ptr<Buffers> > _subdivisionLevels;

	const TerrainTexManager& _terrainTexManager;
	TerrainGeometry&   _terrainGeometry;

	std::vector<igElement*> _trees;
};
