#pragma once
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <iostream>
#include <vector>
#include "chunk.h"
#include "terrainTexManager.h"
#include "map.h"

#define HEIGHT_FACTOR 1000.

typedef struct Region Region;
struct Region {
	int count;
	GLuint* indices;
	int cursor;
};

class Heightmap : public Chunk {

public:
	Heightmap(sf::Vector2i chunkPosition, int _seed, TerrainTexManager* _texManager, Map* _map);
	~Heightmap() {}

	void generate(std::vector<Constraint> constraints);
	void draw() const;
	void saveToImage() const;

	virtual void calculateFrustum(const Camera* camera);

	virtual Constraint getConstraint(sf::Vector2i fromChunkPos) const;
	float getHeight(float x, float y) const; // linear interpolation
	inline int getSize() const {return size;}

private:
	int compareToPoints(sf::Vector3f cam, sf::Vector3f vec, sf::Vector3f* points) const; // 1 if they are all outside, -1 inside, else 0
	int size;
	int seed;

	float* vertices;
	float* normals;
	float* coord;

	sf::Vector3f corners[4]; // For frustum culling
	sf::Vector3f lowests[4];
	sf::Vector3f highests[4];


	std::map<int, Region> regions;

	std::vector<std::vector<float> > heights;
	TerrainTexManager* texManager;
	Map* map;
};
