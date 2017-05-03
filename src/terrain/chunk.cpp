#include "chunk.h"

#include <iostream>

#include "camera.h"
#include "vecUtils.h"

#define BUFFER_OFFSET(a) ((char*)NULL + (a))

Chunk::Chunk(size_t x, size_t y, const TerrainTexManager& terrainTexManager,
	                                     TerrainGeometry&   terrainGeometry) :
	_chunkPos(x,y),
	_visible(false),
	_currentSubdivLvl(1),
  _terrainTexManager(terrainTexManager),
  _terrainGeometry(terrainGeometry) {

	for (size_t i = 0; i < MAX_SUBDIV_LVL+1; i++) {
    _subdivisionLevels.push_back(std::unique_ptr<Buffers>(new Buffers()));
  }
}

void Chunk::cleanSubdivLvl(size_t subdivLvl) {

	glDeleteBuffers(1, &_subdivisionLevels[subdivLvl]->vbo);
	_subdivisionLevels[subdivLvl]->vbo = 0;

	for (auto it  = _subdivisionLevels[subdivLvl]->indicesInfo.begin();
	          it != _subdivisionLevels[subdivLvl]->indicesInfo.end(); it++) {
		glDeleteBuffers(1,&(it->second.ibo));
		it->second.ibo = 0;
		it->second.indices.clear();
	}


	glDeleteVertexArrays(1, &_subdivisionLevels[subdivLvl]->vao);
	_subdivisionLevels[subdivLvl]->vao = 0;
}

Chunk::~Chunk() {
	for (size_t i = 0; i < _subdivisionLevels.size(); i++) {
		cleanSubdivLvl(i);
	}
}

GLuint Chunk::addVertexInfo(Vertex* vertex) {
	Buffers* currentBuffers = _subdivisionLevels[_currentSubdivLvl].get();

	GLuint vertexIndex = currentBuffers->vertices.size()/3;

	for (size_t i = 0; i < currentBuffers->vertices.size(); i+=3) {
		if (currentBuffers->vertices[i] == vertex->pos.x &&
		    currentBuffers->vertices[i+1] == vertex->pos.y &&
				currentBuffers->vertices[i+2] == vertex->pos.z) {

			vertexIndex = i/3;
			break;
		}
	}

	if (vertexIndex == currentBuffers->vertices.size()/3) {
		currentBuffers->vertices.resize(currentBuffers->vertices.size() + 3);
		currentBuffers->normals.resize(currentBuffers->normals.size() + 3);
		currentBuffers->coords.resize(currentBuffers->coords.size() + 2);

		currentBuffers->vertices[currentBuffers->vertices.size()-3] = vertex->pos.x;
		currentBuffers->vertices[currentBuffers->vertices.size()-2] = vertex->pos.y;
		currentBuffers->vertices[currentBuffers->vertices.size()-1] = vertex->pos.z;
		currentBuffers->normals [currentBuffers->normals.size()-3] = vertex->normal.x;
		currentBuffers->normals [currentBuffers->normals.size()-2] = vertex->normal.y;
		currentBuffers->normals [currentBuffers->normals.size()-1] = vertex->normal.z;

		currentBuffers->coords[currentBuffers->coords.size()-2] =
		(vertex->pos.x - CHUNK_SIZE*_chunkPos.x)/CHUNK_SIZE*TEX_FACTOR;
		currentBuffers->coords[currentBuffers->coords.size()-1] =
		(vertex->pos.y - CHUNK_SIZE*_chunkPos.y)/CHUNK_SIZE*TEX_FACTOR;
	}

	return vertexIndex;
}

void Chunk::fillBufferData() {
	std::list<const Triangle*> triangles = _terrainGeometry.getTrianglesInChunk(_chunkPos.x, _chunkPos.y, _currentSubdivLvl);

	for (auto tri = triangles.begin(); tri != triangles.end(); tri++) {
		for (size_t i = 0; i < 3; i++) {
			_subdivisionLevels[_currentSubdivLvl]->indicesInfo[(*tri)->biome]
				.indices.push_back(addVertexInfo((*tri)->vertices[i]));
		}
	}
}

void Chunk::generateBuffers() {
	Buffers* currentBuffers = _subdivisionLevels[_currentSubdivLvl].get();

	// geometry VBO

	glGenBuffers(1, &currentBuffers->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, currentBuffers->vbo);

	size_t bufferSizeVertices = currentBuffers->vertices.size()*sizeof currentBuffers->vertices[0];
	size_t bufferSizeNormals	= currentBuffers->normals. size()*sizeof currentBuffers->normals[0];
	size_t bufferSizeCoords		= currentBuffers->coords.  size()*sizeof currentBuffers->coords[0];

	glBufferData(GL_ARRAY_BUFFER, bufferSizeVertices + bufferSizeNormals + bufferSizeCoords, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, bufferSizeVertices , &currentBuffers->vertices[0]);
  glBufferSubData(GL_ARRAY_BUFFER,    bufferSizeVertices , bufferSizeNormals, &currentBuffers->normals[0]);
	glBufferSubData(GL_ARRAY_BUFFER,    bufferSizeVertices + bufferSizeNormals, bufferSizeCoords, &currentBuffers->coords[0]);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

	// VAO

	glGenVertexArrays(1, &currentBuffers->vao);
	glBindVertexArray(currentBuffers->vao);
	glBindBuffer(GL_ARRAY_BUFFER, currentBuffers->vbo);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(currentBuffers->vertices.size()*sizeof currentBuffers->vertices[0]));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(currentBuffers->vertices.size()*sizeof currentBuffers->vertices[0] +
		              currentBuffers->normals. size()*sizeof currentBuffers->normals[0]));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	// IBO for each biome

	for (auto it = currentBuffers->indicesInfo.begin(); it != currentBuffers->indicesInfo.end(); it++) {
		size_t bufferSizeIndices = it->second.indices.size()*sizeof it->second.indices[0];

		glGenBuffers(1, &(it->second.ibo));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, it->second.ibo);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferSizeIndices, NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, bufferSizeIndices, &(it->second.indices[0]));

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void Chunk::computeChunkBoundingBox() {
	Buffers* currentBuffers = _subdivisionLevels[_currentSubdivLvl].get();

	float minCoord[3];
	float maxCoord[3];

	for (size_t i = 0; i < 3; i++) {
		minCoord[i] = std::numeric_limits<float>::max();
		maxCoord[i] = - std::numeric_limits<float>::max();
	}

	for (size_t i = 0; i < currentBuffers->vertices.size(); i+=3) {
		for (size_t j = 0; j < 3; j++) {
			if (currentBuffers->vertices[i+j] < minCoord[j])
				minCoord[j] = currentBuffers->vertices[i+j];
			if (currentBuffers->vertices[i+j] > maxCoord[j])
				maxCoord[j] = currentBuffers->vertices[i+j];
		}
	}

	currentBuffers->corners[0] = sf::Vector3f(minCoord[0], minCoord[1], minCoord[2]);
	currentBuffers->corners[1] = sf::Vector3f(minCoord[0], minCoord[1], maxCoord[2]);
	currentBuffers->corners[2] = sf::Vector3f(minCoord[0], maxCoord[1], minCoord[2]);
	currentBuffers->corners[3] = sf::Vector3f(minCoord[0], maxCoord[1], maxCoord[2]);
	currentBuffers->corners[4] = sf::Vector3f(maxCoord[0], minCoord[1], minCoord[2]);
	currentBuffers->corners[5] = sf::Vector3f(maxCoord[0], minCoord[1], maxCoord[2]);
	currentBuffers->corners[6] = sf::Vector3f(maxCoord[0], maxCoord[1], minCoord[2]);
	currentBuffers->corners[7] = sf::Vector3f(maxCoord[0], maxCoord[1], maxCoord[2]);
}

void Chunk::generate() {
	if (_subdivisionLevels[_currentSubdivLvl]->vao == 0) {
		fillBufferData();
		generateBuffers();
	}

	computeChunkBoundingBox();
}

size_t Chunk::draw() const {
	Buffers* currentBuffers = _subdivisionLevels[_currentSubdivLvl].get();

	glBindVertexArray(currentBuffers->vao);

	size_t nbTriangles = 0;

	for (auto it = currentBuffers->indicesInfo.begin(); it != currentBuffers->indicesInfo.end(); it++) {
		_terrainTexManager.bindTexture(it->first);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, it->second.ibo);

		glDrawElements(GL_TRIANGLES, it->second.indices.size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0));

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		nbTriangles += it->second.indices.size() / 3;
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	glBindVertexArray(0);

	return nbTriangles;
}

bool Chunk::theCornersAreOutside(sf::Vector3f cam, sf::Vector3f vec) const {
  float dots[8];

  for (size_t i = 0 ; i < 8 ; i++) {
    dots[i] = vu::dot(_subdivisionLevels[_currentSubdivLvl]->corners[i]-cam, vec);

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
void Chunk::setTreesHeights() {
	for (size_t i = 0; i < _trees.size(); i++) {
		_trees[i]->setHeight(getHeight(_trees[i]->getPos()));
	}
}

void Chunk::setTrees(std::vector<igElement*> trees) {
	_trees = trees;
	setTreesHeights();
	_treeDrawer.init(STATIC_DRAW, trees.size());
	_treeDrawer.loadElements(trees);
}

void Chunk::computeSubdivisionLevel() {
	Camera& camera = Camera::getInstance();
	sf::Vector3f centerOfChunk((_chunkPos.x+0.5)*CHUNK_SIZE, (_chunkPos.y+0.5)*CHUNK_SIZE,
	    getHeight(sf::Vector2f((_chunkPos.x+0.5)*CHUNK_SIZE, (_chunkPos.y+0.5)*CHUNK_SIZE)));

	float distanceToChunk = vu::norm(camera.getPos()-centerOfChunk);

	if (distanceToChunk > 10000)
		setSubdivisionLevel(1);
	else if (distanceToChunk > 2500)
		setSubdivisionLevel(2);
	else if (distanceToChunk > 500)
		setSubdivisionLevel(3);
	else
		setSubdivisionLevel(4);
}

void Chunk::setSubdivisionLevel(size_t newSubdLvl) {
	if (newSubdLvl != _currentSubdivLvl) {
		_currentSubdivLvl = newSubdLvl;
		generate();
		setTrees(_trees);
	}
}

float Chunk::getHeight(sf::Vector2f pos) const {
  return _terrainGeometry.getHeight(pos, _currentSubdivLvl);
}
