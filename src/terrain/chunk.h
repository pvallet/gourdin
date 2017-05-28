#pragma once

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/System.hpp>

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

	sf::Vector3f corners[8]; // For frustum culling

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

	float getHeight(sf::Vector2f pos) const;
	sf::Vector3f getNorm(sf::Vector2f pos) const;
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
	bool theCornersAreOutside(sf::Vector3f cam, sf::Vector3f vec) const;
	void setTreesHeights();

	sf::Vector2i _chunkPos;

	bool _visible;
	size_t _currentSubdivLvl;
	std::vector<std::unique_ptr<Buffers> > _subdivisionLevels;

	const TerrainTexManager& _terrainTexManager;
	TerrainGeometry&   _terrainGeometry;

	std::vector<igElement*> _trees;
	igElementDisplay _treeDrawer;
};
