#pragma once

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include <iostream>
#include <vector>

#include "chunk.h"
#include "texManager.h"
#include "map.h"

class Heightmap : public Chunk {

public:
	Heightmap(size_t x, size_t y, const TexManager& terrainTexManager, const Map& map);

	void generate();
	void draw() const;
	void saveToImage() const;

	virtual void computeCulling();

	float getHeight(float x, float y) const; // linear interpolation
	inline int getSize() const {return _size;}

private:
	void getMapInfo();
	void fillBufferData();
	void generateBuffers();
	void computeLowestsHighests();

	size_t _size;

	std::vector<float> _vertices;
	std::vector<float> _coord;
	std::vector<float> _normals;

	std::map<Biome, std::vector<GLuint> > _indices;

	sf::Vector3f _lowests[4]; // To adapt frustum culling
	sf::Vector3f _highests[4];

	std::vector<std::vector<float> > _heights;
	const TexManager& _terrainTexManager;
	const Map& _map;
};
