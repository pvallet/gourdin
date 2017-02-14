#include "chunk.h"

#include <iostream>

#include "camera.h"
#include "vecUtils.h"

#define BUFFER_OFFSET(a) ((char*)NULL + (a))

Chunk::Chunk(size_t x, size_t y, const TerrainTexManager& terrainTexManager,
	                               const TerrainGeometry&   terrainGeometry) :
	_chunkPos(x,y),
	_vao(0),
	_geometryVBO(0),
	_visible(false),
  _terrainTexManager(terrainTexManager),
  _terrainGeometry(terrainGeometry) {
}

Chunk::~Chunk() {
	glDeleteBuffers(1, &_geometryVBO);

	for (auto it = _textureData.begin(); it != _textureData.end(); it++) {
		glDeleteBuffers(1,&(it->second.ibo));
	}
}

GLuint Chunk::addVertexInfo(Vertex* vertex) {
	GLuint vertexIndex = _vertices.size()/3;

	for (size_t i = 0; i < _vertices.size(); i+=3) {
		if (_vertices[i] == vertex->pos.x &&
		    _vertices[i+1] == vertex->pos.y &&
				_vertices[i+2] == vertex->pos.z) {

			vertexIndex = i/3;
			break;
		}
	}

	if (vertexIndex == _vertices.size()/3) {
		_vertices.resize(_vertices.size() + 3);
		_normals.resize(_normals.size() + 3);
		_coords.resize(_coords.size() + 2);

		_vertices[_vertices.size()-3] = vertex->pos.x;
		_vertices[_vertices.size()-2] = vertex->pos.y;
		_vertices[_vertices.size()-1] = vertex->pos.z;
		_normals[_normals.size()-3] = vertex->normal.x;
		_normals[_normals.size()-2] = vertex->normal.y;
		_normals[_normals.size()-1] = vertex->normal.z;

		_coords[_coords.size()-2] = (vertex->pos.x - CHUNK_SIZE*_chunkPos.x)/CHUNK_SIZE*TEX_FACTOR;
		_coords[_coords.size()-1] = (vertex->pos.y - CHUNK_SIZE*_chunkPos.y)/CHUNK_SIZE*TEX_FACTOR;
	}

	return vertexIndex;
}

void Chunk::fillBufferData() {
	std::list<Triangle*> triangles = _terrainGeometry.getTrianglesInChunk(_chunkPos.x, _chunkPos.y);

	for (auto tri = triangles.begin(); tri != triangles.end(); tri++) {
		for (size_t i = 0; i < 3; i++) {
			_textureData[(*tri)->biome].indices.push_back(addVertexInfo((*tri)->vertices[i]));
		}
	}
}

void Chunk::generateBuffers() {
	// geometry VBO

	glGenBuffers(1, &_geometryVBO);
	glBindBuffer(GL_ARRAY_BUFFER, _geometryVBO);

	size_t bufferSizeVertices = _vertices.size()*sizeof _vertices[0];
	size_t bufferSizeNormals	= _normals. size()*sizeof _normals[0];
	size_t bufferSizeCoords		= _coords.  size()*sizeof _coords[0];

	glBufferData(GL_ARRAY_BUFFER, bufferSizeVertices + bufferSizeNormals + bufferSizeCoords, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, bufferSizeVertices , &_vertices[0]);
  glBufferSubData(GL_ARRAY_BUFFER,    bufferSizeVertices , bufferSizeNormals, &_normals[0]);
	glBufferSubData(GL_ARRAY_BUFFER,    bufferSizeVertices + bufferSizeNormals, bufferSizeCoords, &_coords[0]);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

	// VAO

	glGenVertexArrays(1, &_vao);
  glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _geometryVBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(_vertices.size()*sizeof _vertices[0]));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(_vertices.size()*sizeof _vertices[0] + _normals.size()*sizeof _normals[0]));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	// IBO for each biome

	for (auto it = _textureData.begin(); it != _textureData.end(); it++) {
		size_t bufferSizeIndices = it->second.indices.size()*sizeof it->second.indices[0];

		glGenBuffers(1, &(it->second.ibo));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, it->second.ibo);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferSizeIndices, NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, bufferSizeIndices, &(it->second.indices[0]));

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void Chunk::computeChunkBoundingBox() {
	float minCoord[3];
	float maxCoord[3];

	for (size_t i = 0; i < 3; i++) {
		minCoord[i] = std::numeric_limits<float>::max();
		maxCoord[i] = - std::numeric_limits<float>::max();
	}

	for (size_t i = 0; i < _vertices.size(); i+=3) {
		for (size_t j = 0; j < 3; j++) {
			if (_vertices[i+j] < minCoord[j])
				minCoord[j] = _vertices[i+j];
			if (_vertices[i+j] > maxCoord[j])
				maxCoord[j] = _vertices[i+j];
		}
	}

	_corners[0] = sf::Vector3f(minCoord[0], minCoord[1], minCoord[2]);
	_corners[1] = sf::Vector3f(minCoord[0], minCoord[1], maxCoord[2]);
	_corners[2] = sf::Vector3f(minCoord[0], maxCoord[1], minCoord[2]);
	_corners[3] = sf::Vector3f(minCoord[0], maxCoord[1], maxCoord[2]);
	_corners[4] = sf::Vector3f(maxCoord[0], minCoord[1], minCoord[2]);
	_corners[5] = sf::Vector3f(maxCoord[0], minCoord[1], maxCoord[2]);
	_corners[6] = sf::Vector3f(maxCoord[0], maxCoord[1], minCoord[2]);
	_corners[7] = sf::Vector3f(maxCoord[0], maxCoord[1], maxCoord[2]);
}

void Chunk::generate() {
	fillBufferData();
	generateBuffers();
	computeChunkBoundingBox();
}

size_t Chunk::draw() const {
	glBindVertexArray(_vao);

	size_t nbTriangles = 0;

	for (auto it = _textureData.begin(); it != _textureData.end(); it++) {
		_terrainTexManager.bindTexture(it->first);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, it->second.ibo);

		glDrawElements(GL_TRIANGLES, it->second.indices.size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0));

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glBindTexture(GL_TEXTURE_2D, 0);

		nbTriangles += it->second.indices.size() / 3;
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	glBindVertexArray(0);

	return nbTriangles;
}

bool Chunk::theCornersAreOutside(sf::Vector3f cam, sf::Vector3f vec) const {
  float dots[8];

  for (size_t i = 0 ; i < 8 ; i++) {
    dots[i] = vu::dot(_corners[i]-cam, vec);

		if (dots[i] < 0.f)
			return false;
  }

  return true;
}

void Chunk::computeCulling() {
	Camera& cam = Camera::getInstance();

  float theta = cam.getTheta();
  float phi   = cam.getPhi();
  float alpha = cam.getFov() * cam.getRatio() / 2.f;

  // Bottom of the view
  sf::Vector3f norm = vu::carthesian(1.f, theta, phi + 90.f - cam.getFov() / 2.f);
  sf::Vector3f pos = cam.getPos();

  if (theCornersAreOutside(pos,norm)) {
    _visible = false;
    return;
  }

  // Top
  norm = vu::carthesian(1.f, theta, phi + 90.f + cam.getFov() / 2.f);
  norm *= -1.f;

  if (theCornersAreOutside(pos,norm)) {
    _visible = false;
    return;
  }

  // Right
  norm = vu::carthesian(1.f, theta + 90.f, 90.f);
  vu::Mat3f rot;
  rot.rotation(vu::carthesian(1.f, theta + 180.f, 90.f - phi), - alpha);
  norm = rot.multiply(norm);

  if (theCornersAreOutside(pos,norm)) {
    _visible = false;
    return;
  }

  // Left
  norm = vu::carthesian(1.f, theta - 90.f, 90.f);
  rot.rotation(vu::carthesian(1.f, theta + 180.f, 90.f - phi), alpha);
  norm = rot.multiply(norm);

  if (theCornersAreOutside(pos,norm)) {
    _visible = false;
    return;
  }

  _visible = true;
}

float Chunk::getHeight(sf::Vector2f pos) const {
  std::list<Triangle*> toTest = _terrainGeometry.getTrianglesNearPos(pos);

  for (auto tri = toTest.begin(); tri != toTest.end(); tri++) {
    float x[3]; float y[3]; float z[3];

    for (size_t i = 0; i < 3; i++) {
      x[i] = (*tri)->vertices[i]->pos.x;
			y[i] = (*tri)->vertices[i]->pos.y;
			z[i] = (*tri)->vertices[i]->pos.z;
    }

    float s = ((y[1]-y[2])*(pos.x-x[2])+(x[2]-x[1])*(pos.y-y[2])) /
              ((y[1]-y[2])*(x[0]-x[2])+(x[2]-x[1])*(y[0]-y[2]));

    float t = ((y[2]-y[0])*(pos.x-x[2])+(x[0]-x[2])*(pos.y-y[2])) /
              ((y[1]-y[2])*(x[0]-x[2])+(x[2]-x[1])*(y[0]-y[2]));

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1 && s + t <= 1) {
			return s       * z[0] +
	           t       * z[1] +
	           (1-s-t) * z[2];
    }
  }

  return 0;
}
