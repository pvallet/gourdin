#pragma once

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/System.hpp>

#include <vector>

#include "terrainGeometry.h"

#include "terrainTexManager.h"

#include "utils.h"

struct TextureData {
	std::vector<float> distances;
	std::vector<GLuint> indices;

	GLuint ibo;
};

class Chunk {
public:
	Chunk(size_t x, size_t y, const TerrainTexManager& terrainTexManager,
		                        const TerrainGeometry& terrainGeometry);
	virtual ~Chunk();

	void generate();
	size_t draw() const;

	// Set visible to false if there is no need to display the chunk
	virtual void computeCulling();

	float getHeight(sf::Vector2f pos) const;
	inline bool isVisible() const {return _visible;}

protected:
	GLuint addVertexInfo(Vertex* vertex);
	void fillBufferData();
	void generateBuffers();
	void computeChunkBoundingBox();
	bool theCornersAreOutside(sf::Vector3f cam, sf::Vector3f vec) const;

	GLuint _vao;
	GLuint _geometryVBO;

	sf::Vector2i _chunkPos;

	sf::Vector3f _corners[8]; // For frustum culling

	bool _visible;

	std::vector<float> _vertices;
	std::vector<float> _normals;
	std::vector<float> _coords;

	std::map<Biome, TextureData> _textureData;

	const TerrainTexManager& _terrainTexManager;
	const TerrainGeometry&   _terrainGeometry;
};
