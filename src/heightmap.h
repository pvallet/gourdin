#pragma once
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <iostream>
#include <vector>
#include "chunk.h"

#define HEIGHT_FACTOR 50.


class Heightmap : public Chunk {

public:
	Heightmap(sf::Vector2i chunkPosition, int _seed);
	~Heightmap() {}

	void generate();
	void draw() const;
	void saveToImage() const;

	virtual bool calculateFrustum(const Camera* camera);

	float getHeight(float x, float y) const; // linear interpolation
	inline int getSize() const {return size;}

private:
	int size;
	int seed;

	std::vector<std::vector<float> > heights;
};

