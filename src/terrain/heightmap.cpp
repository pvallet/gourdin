#include "heightmap.h"

#include <iostream>
#include <sstream>
#include <omp.h>

#include "camera.h"
#include "vecUtils.h"

#define DEFAULT_CHUNK_SUBD 33 // +1 to join with other chunks
#define HEIGHT_FACTOR 1000.f

#define BUFFER_OFFSET(a) ((char*)NULL + (a))


Heightmap::Heightmap(size_t x, size_t y, const TexManager& terrainTexManager, const Map& map) :
	Chunk(x,y),
	_size(DEFAULT_CHUNK_SUBD),
  _terrainTexManager(terrainTexManager),
  _map(map) {

  for (unsigned int i = 0 ; i < _size ; i++) {
		_heights.push_back(std::vector<float>(_size, 0.f));
	}

	_vertices.resize(3*_size*_size);
	_coord	 .resize(2*_size*_size);
	_normals .resize(3*_size*_size);
}

Heightmap::~Heightmap() {
	for (auto it = _transitionData.begin(); it != _transitionData.end(); it++) {
		glDeleteBuffers(1,&(it->second.vbo));
		glDeleteBuffers(1,&(it->second.ibo));
	}
}

std::vector<std::vector<Biome> > Heightmap::getMapInfo() {
	size_t size1 = _size-1;
	float step =  CHUNK_SIZE / (float) size1;

  Center* tmpRegions[_size][_size];
	std::vector<Biome> initBiome(_size);
	std::vector<std::vector<Biome> > biomes(_size, initBiome);

  float x, x1, x2, x3;
  float y, y1, y2, y3;
  x = _chunkPos.x * CHUNK_SIZE;

  for (size_t i = 0 ; i < _size ; i++) {
    y = _chunkPos.y * CHUNK_SIZE;

    for (size_t j = 0 ; j < _size ; j++) {
			Center* currentCenter = _map.getClosestCenter(sf::Vector2f(x, y));
      tmpRegions[i][j] = currentCenter;

			bool tryNextAdjacentCenter = true;
			size_t currentAdjacentCenter = 0;
			do {
				if (currentAdjacentCenter == currentCenter->centers.size())
					tryNextAdjacentCenter = false;

				x1 = tmpRegions[i][j]->x;
	      y1 = tmpRegions[i][j]->y;

	      for (unsigned int k = 0 ; k < tmpRegions[i][j]->edges.size() ; k++) {
	        if (!tmpRegions[i][j]->edges[k]->mapEdge) {

						x2 = tmpRegions[i][j]->edges[k]->corner0->x;
	          y2 = tmpRegions[i][j]->edges[k]->corner0->y;
	          x3 = tmpRegions[i][j]->edges[k]->corner1->x;
	          y3 = tmpRegions[i][j]->edges[k]->corner1->y;

						// Linear interpolation to get the height
	          float s = ((y2-y3)*(x-x3)+(x3-x2)*(y-y3)) / ((y2-y3)*(x1-x3)+(x3-x2)*(y1-y3));
	          float t = ((y3-y1)*(x-x3)+(x1-x3)*(y-y3)) / ((y2-y3)*(x1-x3)+(x3-x2)*(y1-y3));

	          if (s >= 0 && s <= 1 && t >= 0 && t <= 1 && s + t <= 1) {
	            _heights[i][j] = s       * tmpRegions[i][j]->elevation +
	                             t       * tmpRegions[i][j]->edges[k]->corner0->elevation +
	                             (1-s-t) * tmpRegions[i][j]->edges[k]->corner1->elevation;
	            if (_heights[i][j] < 0)
	              	_heights[i][j] = 0;

	            _heights[i][j] *=  HEIGHT_FACTOR;
							tryNextAdjacentCenter = false;
							break;
	          }
	        }
				}

				if (tryNextAdjacentCenter) {
					tmpRegions[i][j] = currentCenter->centers[currentAdjacentCenter];
					currentAdjacentCenter++;
				}
			} while (tryNextAdjacentCenter);
			biomes[i][j] = tmpRegions[i][j]->biome;
      y += step;
    }

    x += step;
  }

	return biomes;
}

void Heightmap::fillBufferData() {
	size_t size1 = _size-1;
	float step =  CHUNK_SIZE / (float) size1;

	sf::Vector3f normalsTmp[_size][_size];

  // Compute normals

  sf::Vector3f normal;
  sf::Vector3f neighbourVec[4];

  for (size_t i = 1 ; i < size1 ; i++) {
    for (size_t j = 1 ; j < size1 ; j++) {
      neighbourVec[0] = sf::Vector3f(step,  0.f,   _heights[i-1][j] - _heights[i][j]);
      neighbourVec[1] = sf::Vector3f(0.f,   step,  _heights[i][j-1] - _heights[i][j]);
      neighbourVec[2] = sf::Vector3f(-step, 0.f,   _heights[i+1][j] - _heights[i][j]);
      neighbourVec[3] = sf::Vector3f(0.f,   -step, _heights[i][j+1] - _heights[i][j]);

      normal = vu::cross(neighbourVec[0], neighbourVec[1]) +
               vu::cross(neighbourVec[1], neighbourVec[2]) +
               vu::cross(neighbourVec[2], neighbourVec[3]) +
               vu::cross(neighbourVec[3], neighbourVec[0]);

      normal /= vu::norm(normal);

      normalsTmp[i][j] = normal;
    }
  }

	// We don't have info on the neighbour chunks so we use the closest defined
	// normals for the vertices on the edges
  for (size_t i = 1 ; i < size1 ; i++) {
    normalsTmp[0][i]     = normalsTmp[1][i];
    normalsTmp[size1][i] = normalsTmp[_size-2][i];
    normalsTmp[i][0]     = normalsTmp[i][1];
    normalsTmp[i][size1] = normalsTmp[i][_size-2];
  }

  normalsTmp[0][0] = normalsTmp[0][1];
  normalsTmp[0][size1] = normalsTmp[1][size1];
  normalsTmp[size1][0] = normalsTmp[size1][1];
  normalsTmp[size1][size1] = normalsTmp[size1][_size-2];

	// Store the info for ont the
  #pragma omp parallel for
	for (size_t i = 0 ; i < _size ; i++) {
		for (size_t j = 0 ; j < _size ; j++) {
      _coord[2*i*_size + 2*j]     = (float) i / (float) size1 * TEX_FACTOR;
      _coord[2*i*_size + 2*j + 1] = (float) j / (float) size1 * TEX_FACTOR;

      _vertices[3*i*_size + 3*j] 		 = i * step;
      _vertices[3*i*_size + 3*j + 1] = j * step;
      _vertices[3*i*_size + 3*j + 2] = _heights[i][j];

      _normals[3*i*_size + 3*j]     = normalsTmp[i][j].x;
      _normals[3*i*_size + 3*j + 1] = normalsTmp[i][j].y;
      _normals[3*i*_size + 3*j + 2] = normalsTmp[i][j].z;
    }
  }
}

void Heightmap::addPointToTransitionData(Biome biome, size_t i, size_t j, float distance) {
	// Copy info from generated map
	for (size_t k = 0; k < 3; k++) {
		_transitionData[biome].vertices.push_back(_vertices[3*i*_size + 3*j + k]);
		_transitionData[biome].normals.push_back(_normals[3*i*_size + 3*j + k]);
	}
	for (size_t k = 0; k < 2; k++) {
		_transitionData[biome].coord.push_back(_coord[2*i*_size + 2*j + k]);
	}

	_transitionData[biome].distances.push_back(distance / TERRAIN_TEX_TRANSITION_SIZE / 2 + 0.5);
}

void Heightmap::correctDistance(Biome biome, size_t k, float distance) {
	_transitionData[biome].distances.end()[-4+k] = std::min(
	_transitionData[biome].distances.end()[-4+k],
	(float) (distance / TERRAIN_TEX_TRANSITION_SIZE / 2 + 0.5));
}

// Fill vertices, tex coord and normals that will be fed to the shader handling the transition between textures
std::vector<std::vector<bool> > Heightmap::computeTransitionData() {
	size_t size1 = _size - 1;
	float step =  CHUNK_SIZE / (float) size1;
	std::vector<bool> initPlainTexture(size1,true);
	std::vector<std::vector<bool> > isPlainTexture(size1,initPlainTexture);

	const std::set<Edge*>& edgesInChunk = _map.getEdgesInChunk(_chunkPos.x, _chunkPos.y);

	sf::Vector2f chunkCoord(CHUNK_SIZE*_chunkPos.x, CHUNK_SIZE*_chunkPos.y);

	for (auto edge = edgesInChunk.begin(); edge != edgesInChunk.end(); edge++) {
		for (size_t i = std::max(0,     (int)    (((*edge)->beginX - chunkCoord.x) / step));
						    i < std::min(size1, (size_t) (((*edge)->endX   - chunkCoord.x) / step)+1); i++) {
		for (size_t j = std::max(0,     (int)    (((*edge)->beginY - chunkCoord.y) / step));
		            j < std::min(size1, (size_t) (((*edge)->endY   - chunkCoord.y) / step)+1); j++) {

			size_t currentI[4] = {i, i+1, i, i+1};
			size_t currentJ[4] = {j, j, j+1, j+1};

			sf::Vector2f pointCoord[4];
			float distance[4];
			bool isJoint = false;

			for (size_t k = 0; k < 4; k++) {
				pointCoord[k] = sf::Vector2f(chunkCoord.x + step * currentI[k], chunkCoord.y + step * currentJ[k]);

				if (vu::norm(pointCoord[k] - sf::Vector2f((*edge)->corner0->x, (*edge)->corner0->y)) <= TERRAIN_TEX_TRANSITION_SIZE ||
				    vu::norm(pointCoord[k] - sf::Vector2f((*edge)->corner1->x, (*edge)->corner1->y)) <= TERRAIN_TEX_TRANSITION_SIZE) {
					isJoint = true;
					isPlainTexture[i][j] = false;
					break;
				}

				else {
					distance[k] = (*edge)->getDistanceToEdge(pointCoord[k]);

					if (distance[k] <= TERRAIN_TEX_TRANSITION_SIZE)
						isPlainTexture[i][j] = false;
				}
			}

			if (!isPlainTexture[i][j] && !isJoint) {
				for (size_t k = 0; k < 4; k++) {
					Biome firstBiome;
					// The first biome should have alpha = 1 to avoid blending with the background
					if ((*edge)->center0->biome <= (*edge)->center1->biome)
						firstBiome  = (*edge)->center0->biome;
					else
						firstBiome  = (*edge)->center1->biome;

					if ((*edge)->isOnSameSideAsCenter0(pointCoord[k])) {
						addPointToTransitionData((*edge)->center0->biome, currentI[k],currentJ[k],
							(*edge)->center0->biome != firstBiome ? distance[k] : TERRAIN_TEX_TRANSITION_SIZE);
						addPointToTransitionData((*edge)->center1->biome, currentI[k],currentJ[k],
							(*edge)->center1->biome != firstBiome ? - distance[k] : TERRAIN_TEX_TRANSITION_SIZE);
					}

					else {
						addPointToTransitionData((*edge)->center0->biome, currentI[k],currentJ[k],
							(*edge)->center0->biome != firstBiome ? - distance[k] : TERRAIN_TEX_TRANSITION_SIZE);
						addPointToTransitionData((*edge)->center1->biome, currentI[k],currentJ[k],
							(*edge)->center1->biome != firstBiome ? distance[k] : TERRAIN_TEX_TRANSITION_SIZE);
					}
				}
			}
		}
		}
	}

	return isPlainTexture;
}

void Heightmap::computeJoints() {
	size_t size1 = _size - 1;
	float step =  CHUNK_SIZE / (float) size1;

	const std::set<Edge*>& edgesInChunk = _map.getEdgesInChunk(_chunkPos.x, _chunkPos.y);
	const std::vector<Corner*>& cornersInChunk = _map.getCornersInChunk(_chunkPos.x, _chunkPos.y);
	sf::Vector2f chunkCoord(CHUNK_SIZE*_chunkPos.x, CHUNK_SIZE*_chunkPos.y);

	for (auto corner = cornersInChunk.begin(); corner != cornersInChunk.end(); corner++) {
		std::vector<Edge*> edgesAround;

		for (size_t i = 0; i < (*corner)->edges.size(); i++) {
			if (edgesInChunk.find((*corner)->edges[i]) != edgesInChunk.end())
				edgesAround.push_back((*corner)->edges[i]);
		}

		for (size_t i = std::max(0,     (int)    (((*corner)->x - TERRAIN_TEX_TRANSITION_SIZE - chunkCoord.x) / step));
						    i < std::min(size1, (size_t) (((*corner)->x + TERRAIN_TEX_TRANSITION_SIZE - chunkCoord.x) / step)+1); i++) {
		for (size_t j = std::max(0,     (int)    (((*corner)->y - TERRAIN_TEX_TRANSITION_SIZE - chunkCoord.y) / step));
		            j < std::min(size1, (size_t) (((*corner)->y + TERRAIN_TEX_TRANSITION_SIZE - chunkCoord.y) / step)+1); j++) {

			// Find the biome that will be drawn first to draw it without alpha blending
			Biome firstBiome = (*corner)->centers[0]->biome;

			for (size_t k = 0; k < (*corner)->centers.size(); k++) {
				if ((*corner)->centers[k]->biome < firstBiome)
					firstBiome = (*corner)->centers[k]->biome;
			}

			// Find if we are in a situation handled separately
			std::vector<Biome> notFirstBiome;
			for (size_t k = 0; k < (*corner)->centers.size(); k++) {
				if ((*corner)->centers[k]->biome != firstBiome)
					notFirstBiome.push_back((*corner)->centers[k]->biome);
			}

			bool needToComputeMax = (notFirstBiome.size() == 2 && notFirstBiome[0] == notFirstBiome[1]);

			size_t currentI[4] = {i, i+1, i, i+1};
			size_t currentJ[4] = {j, j, j+1, j+1};

			sf::Vector2f pointCoord[4];
			// Compute positions of the corners and send the first square with the non alpha texture
			for (size_t k = 0; k < 4; k++) {
				pointCoord[k] = sf::Vector2f(chunkCoord.x + step * currentI[k], chunkCoord.y + step * currentJ[k]);
				addPointToTransitionData(firstBiome, currentI[k],currentJ[k], TERRAIN_TEX_TRANSITION_SIZE);
			}

			std::set<Biome> alreadyProcessed;

			for (auto edge = edgesAround.begin(); edge != edgesAround.end(); edge++ {

				for (size_t k = 0; k < 4; k++) {
					float distance = (*edge)->getDistanceToEdge(pointCoord[k]);

					// When a junction has 2 times firstBiome or 3 different biomes, we need to
					// compute the min of the distances yielded by the edges
					// When a junction has 1 firstBiome and 2 times another one, we need to
					// compute the max
					if ((*edge)->isOnSameSideAsCenter0(pointCoord[k])) {
						if ((*edge)->center0->biome != firstBiome) {
							if (alreadyProcessed.find((*edge)->center0->biome) == alreadyProcessed.end() || needToComputeMax)
								addPointToTransitionData((*edge)->center0->biome, currentI[k],currentJ[k], distance);
							else
								correctDistance((*edge)->center0->biome, k, distance);
						}

						if ((*edge)->center1->biome != firstBiome) {
							if (alreadyProcessed.find((*edge)->center1->biome) == alreadyProcessed.end() || needToComputeMax)
								addPointToTransitionData((*edge)->center1->biome, currentI[k],currentJ[k], -distance);
							else
								correctDistance((*edge)->center1->biome, k, -distance);
						}
					}

					else {
						if ((*edge)->center0->biome != firstBiome) {
							if (alreadyProcessed.find((*edge)->center0->biome) == alreadyProcessed.end() || needToComputeMax)
								addPointToTransitionData((*edge)->center0->biome, currentI[k],currentJ[k], -distance);
							else
								correctDistance((*edge)->center0->biome, k, -distance);
						}

						if ((*edge)->center1->biome != firstBiome) {
							if (alreadyProcessed.find((*edge)->center1->biome) == alreadyProcessed.end() || needToComputeMax)
								addPointToTransitionData((*edge)->center1->biome, currentI[k],currentJ[k], distance);
							else
								correctDistance((*edge)->center1->biome, k, distance);
						}
					}
				}

				alreadyProcessed.insert((*edge)->center0->biome);
				alreadyProcessed.insert((*edge)->center1->biome);
			}
		}
		}
	}
}

void Heightmap::computeIndices(const std::vector<std::vector<Biome> >& biomes,
	                             const std::vector<std::vector<bool> >& isPlainTexture) {

	// Plain texture indices
	for (size_t i = 0 ; i < _size-1 ; i++) {
		for (size_t j = 0 ; j < _size-1 ; j++) {
			if (isPlainTexture[i][j]) {
				// Set the indices for the two triangles that will constitute a square which
				// top left corner will be of position (i,j)

				std::vector<GLuint>& currentIndices = _indicesPlainTexture[biomes[i][j]];

				currentIndices.push_back( 		i*_size + j);
				currentIndices.push_back( (i+1)*_size + j);
				currentIndices.push_back( 		i*_size + j+1);

				currentIndices.push_back( 		i*_size + j+1);
				currentIndices.push_back( (i+1)*_size + j);
				currentIndices.push_back( (i+1)*_size + j+1);
			}
		}
	}

	// Indices for transition between textures
	// The vertices are stored forming squares, with duplicates. We only need to join them:
	// 0 --- 2
	// |     |  yields 0 1 2 2 1 3
	// 1 --- 3
	for (auto it = _transitionData.begin(); it != _transitionData.end(); it++) {
		for (size_t i = 0; i < it->second.vertices.size() / 3; i+=4) {
			it->second.indices.push_back(i);
			it->second.indices.push_back(i+1);
			it->second.indices.push_back(i+2);
			it->second.indices.push_back(i+2);
			it->second.indices.push_back(i+1);
			it->second.indices.push_back(i+3);
		}
	}
}

void Heightmap::generateBuffersPlain() {
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);

	size_t bufferSizeVertices = _vertices.size()*sizeof _vertices[0];
	size_t bufferSizeCoord		= _coord.   size()*sizeof _coord[0];
	size_t bufferSizeNormals	= _normals. size()*sizeof _normals[0];

  glBufferData(GL_ARRAY_BUFFER, bufferSizeVertices + bufferSizeCoord + bufferSizeNormals, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, bufferSizeVertices , &_vertices[0]);
	glBufferSubData(GL_ARRAY_BUFFER,    bufferSizeVertices , bufferSizeCoord, &_coord[0]);
  glBufferSubData(GL_ARRAY_BUFFER,    bufferSizeVertices + bufferSizeCoord, bufferSizeNormals, &_normals[0]);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*(_size-1)*(_size-1) * sizeof(GLuint), NULL, GL_STATIC_DRAW);

  int cursor = 0;
  for (auto it = _indicesPlainTexture.begin(); it != _indicesPlainTexture.end() ; ++it ) {
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, cursor, it->second.size() * sizeof(GLuint), &(it->second[0]));
    cursor += it->second.size() * sizeof(GLuint);
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	_glCheckError();
}

void Heightmap::generateBuffersTransition() {
	for (auto it = _transitionData.begin(); it != _transitionData.end(); it++) {
		glGenBuffers(1, &(it->second.vbo));
		glBindBuffer(GL_ARRAY_BUFFER, it->second.vbo);

		size_t sizeVertices = it->second.vertices. size()*sizeof it->second.vertices[0];
		size_t sizeCoord		= it->second.coord.    size()*sizeof it->second.coord[0];
		size_t sizeNormals	= it->second.normals.  size()*sizeof it->second.normals[0];
		size_t sizeDistances= it->second.distances.size()*sizeof it->second.distances[0];
		size_t sizeIndices	= it->second.indices.  size()*sizeof it->second.indices[0];

		glBufferData(GL_ARRAY_BUFFER, sizeVertices + sizeCoord + sizeNormals + sizeDistances, NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeVertices , &(it->second.vertices[0]));
		glBufferSubData(GL_ARRAY_BUFFER,    sizeVertices , sizeCoord , &(it->second.coord[0]));
		glBufferSubData(GL_ARRAY_BUFFER,    sizeVertices + sizeCoord , sizeNormals , &(it->second.normals[0]));
		glBufferSubData(GL_ARRAY_BUFFER,    sizeVertices + sizeCoord + sizeNormals , sizeDistances, &(it->second.distances[0]));

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &(it->second.ibo));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, it->second.ibo);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeIndices, NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeIndices, &(it->second.indices[0]));

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		_glCheckError();
	}
}

void Heightmap::computeLowestsHighests() {
	size_t size1 = _size-1;
	float step =  CHUNK_SIZE / (float) size1;
	float minH = HEIGHT_FACTOR;
	float maxH = 0.f;
  int iMin = 0;
  int iMax = 0;

  for (size_t i = 0 ; i < _size ; i++) {
    if (_heights[0][i] < minH) {
      iMin = i;
      minH = _heights[0][i];
    }

    if (_heights[0][i] > maxH) {
      iMax = i;
      maxH = _heights[0][i];
    }
  }

  _lowests[0]  = sf::Vector3f(_chunkPos.x * CHUNK_SIZE, _chunkPos.y * CHUNK_SIZE + iMin * step, _heights[0][iMin]);
  _highests[0] = sf::Vector3f(_chunkPos.x * CHUNK_SIZE, _chunkPos.y * CHUNK_SIZE + iMax * step, _heights[0][iMax]);

	minH = HEIGHT_FACTOR;
	maxH = 0.f;
  for (size_t i = 0 ; i < _size ; i++) {
    if (_heights[size1][i] < minH) {
      iMin = i;
      minH = _heights[size1][i];
    }

    if (_heights[size1][i] > maxH) {
      iMax = i;
      maxH = _heights[size1][i];
    }
  }

  _lowests[1]  = sf::Vector3f((_chunkPos.x+1) * CHUNK_SIZE, _chunkPos.y * CHUNK_SIZE + iMin * step, _heights[size1][iMin]);
  _highests[1] = sf::Vector3f((_chunkPos.x+1) * CHUNK_SIZE, _chunkPos.y * CHUNK_SIZE + iMax * step, _heights[size1][iMax]);

	minH = HEIGHT_FACTOR;
	maxH = 0.f;
  for (size_t i = 0 ; i < _size ; i++) {
    if (_heights[i][0] < minH) {
      iMin = i;
      minH = _heights[i][0];
    }

    if (_heights[i][0] > maxH) {
      iMax = i;
      maxH = _heights[i][0];
    }
  }

  _lowests[2]  = sf::Vector3f(_chunkPos.x * CHUNK_SIZE + iMin * step, _chunkPos.y * CHUNK_SIZE, _heights[iMin][0]);
  _highests[2] = sf::Vector3f(_chunkPos.x * CHUNK_SIZE + iMax * step, _chunkPos.y * CHUNK_SIZE, _heights[iMax][0]);

	minH = HEIGHT_FACTOR;
	maxH = 0.f;
  for (size_t i = 0 ; i < _size ; i++) {
    if (_heights[i][size1] < minH) {
      iMin = i;
      minH = _heights[i][size1];
    }

    if (_heights[i][size1] > maxH) {
      iMax = i;
      maxH = _heights[i][size1];
    }
  }

  _lowests[3]  = sf::Vector3f(_chunkPos.x * CHUNK_SIZE + iMin * step, (_chunkPos.y+1) * CHUNK_SIZE,  _heights[iMin][size1]);
  _highests[3] = sf::Vector3f(_chunkPos.x * CHUNK_SIZE + iMax * step, (_chunkPos.y+1) * CHUNK_SIZE,  _heights[iMax][size1]);
}

void Heightmap::generate() {
  size_t size1 = _size-1;
	float step =  CHUNK_SIZE / (float) size1;

  std::vector<std::vector<Biome> > biomeInfo = getMapInfo();

	fillBufferData();

	std::vector<std::vector<bool> > plainTextureInfo = computeTransitionData();

	computeJoints();

	computeIndices(biomeInfo, plainTextureInfo);

	generateBuffersPlain();
	generateBuffersTransition();

  _corners[0] = sf::Vector3f(_chunkPos.x * CHUNK_SIZE, _chunkPos.y * CHUNK_SIZE, _heights[0][0]);
	_corners[1] = sf::Vector3f((_chunkPos.x+1) * CHUNK_SIZE, (_chunkPos.y+1) * CHUNK_SIZE, _heights[size1][size1]);
  _corners[2] = sf::Vector3f(_chunkPos.x * CHUNK_SIZE, (_chunkPos.y+1) * CHUNK_SIZE, _heights[0][size1]);
  _corners[3] = sf::Vector3f((_chunkPos.x+1) * CHUNK_SIZE, _chunkPos.y * CHUNK_SIZE, _heights[size1][0]);

	computeLowestsHighests();
}

void Heightmap::draw() const {
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(_vertices.size()*sizeof _vertices[0]));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(_vertices.size()*sizeof _vertices[0] + _coord.size()*sizeof _coord[0]));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

  int cursor = 0;
  for (auto it = _indicesPlainTexture.begin(); it != _indicesPlainTexture.end() ; ++it ) {
    _terrainTexManager.bindTexture(it->first);

    glDrawElements(GL_TRIANGLES, it->second.size(), GL_UNSIGNED_INT, BUFFER_OFFSET(cursor));
    cursor += it->second.size() * sizeof(GLuint);
  }

	glBindTexture(GL_TEXTURE_2D, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Heightmap::drawTransitions() const {
	for (auto it = _transitionData.begin(); it != _transitionData.end(); it++) {
		_terrainTexManager.bindTexture(it->first);

		glBindBuffer(GL_ARRAY_BUFFER, it->second.vbo);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(it->second.vertices.size()*sizeof it->second.vertices[0]));

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(it->second.vertices.size()*sizeof it->second.vertices[0] +
			              it->second.coord.   size()*sizeof it->second.coord[0]));

		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(it->second.vertices.size()*sizeof it->second.vertices[0] +
			              it->second.coord.   size()*sizeof it->second.coord[0] +
									  it->second.normals. size()*sizeof it->second.normals[0]));

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, it->second.ibo);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);

		glDrawElements(GL_TRIANGLES, it->second.indices.size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0));

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void Heightmap::saveToImage() const {
	sf::Uint8* pixels = new sf::Uint8[_size * _size * 4];

	for (size_t i = 0 ; i < _size ; i++) { // Convert _heights to array of pixels
		for (size_t j = 0 ; j < _size ; j++) {
			for (size_t k = 0 ; k < 3 ; k++) {
				pixels[i*4*_size + j*4 + k] = _heights[i][j]*255 / HEIGHT_FACTOR;
			}
			pixels[i*4*_size + j*4 + 3] = 255;
		}
	}

	sf::Texture texture;
	texture.create(_size, _size);
	texture.update(pixels);

  std::ostringstream convert;
  convert << _chunkPos.x << "." << _chunkPos.y << ".png";

	texture.copyToImage().saveToFile(convert.str());
}

void Heightmap::computeCulling() {
	Camera& cam = Camera::getInstance();

  float theta = cam.getTheta();
  float phi   = cam.getPhi();
  float alpha = cam.getFov() * cam.getRatio() / 2.f;

  // Bottom of the view
  sf::Vector3f norm = vu::carthesian(1.f, theta, phi + 90.f - cam.getFov() / 2.f);
  sf::Vector3f pos = cam.getPos();

  if (compareToPoints(pos,norm,_corners) == 1 &&
			compareToPoints(pos,norm,_highests) == 1) {
    _visible = false;
    return;
  }

  // Top
  norm = vu::carthesian(1.f, theta, phi + 90.f + cam.getFov() / 2.f);
  norm *= -1.f;

  if (compareToPoints(pos,norm,_corners) == 1 &&
			compareToPoints(pos,norm,_lowests) == 1) {
    _visible = false;
    return;
  }

  // Right
  norm = vu::carthesian(1.f, theta + 90.f, 90.f);
  vu::Mat3f rot;
  rot.rotation(vu::carthesian(1.f, theta + 180.f, 90.f - phi), - alpha);
  norm = rot.multiply(norm);

  if (compareToPoints(pos,norm,_corners) == 1) {
    _visible = false;
    return;
  }

  // Left
  norm = vu::carthesian(1.f, theta - 90.f, 90.f);
  rot.rotation(vu::carthesian(1.f, theta + 180.f, 90.f - phi), alpha);
  norm = rot.multiply(norm);

  if (compareToPoints(pos,norm,_corners) == 1) {
    _visible = false;
    return;
  }

  _visible = true;
}

float Heightmap::getHeight(float x, float y) const {
  x *= (_size - 1) / CHUNK_SIZE;
  y *= (_size - 1) / CHUNK_SIZE;

  int iX = x;
  int iY = y;

  float fX = x - iX;
  float fY = y - iY;

  return  _heights[iX][iY] +
          (_heights[iX+1][iY] - _heights[iX][iY]) * fX +
          (_heights[iX][iY+1] - _heights[iX][iY]) * fY +
          (_heights[iX+1][iY+1] + _heights[iX][iY] - _heights[iX+1][iY] - _heights[iX][iY+1]) * fX * fY;
}
