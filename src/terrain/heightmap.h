#pragma once
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <iostream>
#include <vector>
#include "chunk.h"
#include "texManager.h"
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
	Heightmap(sf::Vector2i chunkPosition, int seed, TexManager* terrainTexManager, Map* map);

	void generate(std::vector<Constraint> constraints);
	void draw() const;
	void saveToImage() const;

	virtual void calculateFrustum(const Camera* camera);

	virtual Constraint getConstraint(sf::Vector2i fromChunkPos) const;
	float getHeight(float x, float y) const; // linear interpolation
	inline int getSize() const {return _size;}

private:
	int compareToPoints(sf::Vector3f cam, sf::Vector3f vec, sf::Vector3f* points) const; // 1 if they are all outside, -1 inside, else 0
	size_t _size;
	int _seed;

	std::vector<float> _vertices;
	std::vector<float> _coord;
	std::vector<float> _normals;

	sf::Vector3f _corners[4]; // For frustum culling
	sf::Vector3f _lowests[4];
	sf::Vector3f _highests[4];

	std::map<int, Region> _regions;

	std::vector<std::vector<float> > _heights;
	TexManager* _terrainTexManager;
	Map* _map;
};
