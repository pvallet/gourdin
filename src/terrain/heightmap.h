#pragma once

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include <iostream>
#include <vector>

#include "chunk.h"
#include "texManager.h"
#include "map.h"

struct TransitionData {
	std::vector<float> vertices;
	std::vector<float> coord;
	std::vector<float> normals;
	std::vector<float> distances;
	std::vector<GLuint> indices;

	GLuint vbo;
	GLuint ibo;
};

class Heightmap : public Chunk {

public:
	Heightmap(size_t x, size_t y, const TexManager& terrainTexManager, const Map& map);
	~Heightmap();

	void generate();
	void draw() const;
	void drawTransitions() const;
	void saveToImage() const;

	virtual void computeCulling();

	float getHeight(float x, float y) const; // linear interpolation
	inline int getSize() const {return _size;}

private:
	std::vector<std::vector<Biome> > getMapInfo();
	void fillBufferData();

	void addPointToTransitionData(Biome biome, size_t i, size_t j, float distance);
	std::vector<std::vector<bool> > computeTransitionData();
	void computeIndices(const std::vector<std::vector<Biome> >& biomes,
		                  const std::vector<std::vector<bool> >& isPlainTexture);

	void generateBuffersPlain();
	void generateBuffersTransition();
	void computeLowestsHighests();

	size_t _size;

	std::vector<float> _vertices;
	std::vector<float> _coord;
	std::vector<float> _normals;

	std::map<Biome, std::vector<GLuint> > _indicesPlainTexture;

	std::map<Biome, TransitionData> _transitionData;

	sf::Vector3f _lowests[4]; // To adapt frustum culling
	sf::Vector3f _highests[4];

	std::vector<std::vector<float> > _heights;
	const TexManager& _terrainTexManager;
	const Map& _map;
};
