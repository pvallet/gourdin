#pragma once

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/System.hpp>

#include <vector>

#include "map.h"

#include "texManager.h"

#include "utils.h"

struct TransitionData {
	std::vector<float> vertices;
	std::vector<float> coord;
	std::vector<float> normals;
	std::vector<float> distances;
	std::vector<GLuint> indices;

	GLuint vbo;
	GLuint ibo;
};

class Chunk {
public:
	Chunk(size_t x, size_t y, const TexManager& terrainTexManager, const Map& map);
	virtual ~Chunk();

	void generate();
	void draw() const;
	void drawTransitions() const;

	// Set visible to false if there is no need to display the chunk
	virtual void computeCulling();

	inline GLuint getVBOIndex() const {return _vbo;}
	inline GLuint getIBOIndex() const {return _ibo;}

	float getHeight(float x, float y) const;
	inline int getSize() const {return _size;}
	inline bool isVisible() const {return _visible;}

protected:
	std::vector<std::vector<Biome> > getMapInfo();
	void fillBufferData();

	void addPointToTransitionData(Biome biome, size_t i, size_t j, float distance);
	void correctDistance(Biome biome, size_t k, float distance);
	std::vector<std::vector<bool> > computeTransitionData();
	void computeJoints();
	void computeIndices(const std::vector<std::vector<Biome> >& biomes,
		                  const std::vector<std::vector<bool> >& isPlainTexture);

	void generateBuffersPlain();
	void generateBuffersTransition();
	void computeLowestsHighests();

	// 1 if they are all outside, -1 inside, else 0
	int compareToPoints(sf::Vector3f cam, sf::Vector3f vec, sf::Vector3f* points) const;

	GLuint _vbo;
	GLuint _ibo;

	sf::Vector2i _chunkPos;

	sf::Vector3f _corners[4]; // For frustum culling
	sf::Vector3f _lowests[4]; // To adapt frustum culling
	sf::Vector3f _highests[4];

	bool _visible;

	size_t _size;

	std::vector<float> _vertices;
	std::vector<float> _coord;
	std::vector<float> _normals;

	std::map<Biome, std::vector<GLuint> > _indicesPlainTexture;

	std::map<Biome, TransitionData> _transitionData;

	std::vector<std::vector<float> > _heights;
	const TexManager& _terrainTexManager;
	const Map& _map;
};
