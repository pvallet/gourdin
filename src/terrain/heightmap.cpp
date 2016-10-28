#include "heightmap.h"

#include <iostream>
#include <sstream>
#include <omp.h>

#include "camera.h"
#include "vecUtils.h"

#define DEFAULT_CHUNK_SUBD 33 // +1 to join with other chunks
#define BUFFER_OFFSET(a) ((char*)NULL + (a))

#define SMOOTH_RANGE 20. // Percentage


Heightmap::Heightmap(sf::Vector2i chunkPosition, int seed, const TexManager& terrainTexManager, const Map& map) :
	Chunk(chunkPosition),
	_size(DEFAULT_CHUNK_SUBD),
  _seed(seed),
  _terrainTexManager(terrainTexManager),
  _map(map) {

  for (unsigned int i = 0 ; i < _size ; i++) {
		_heights.push_back(std::vector<float>(_size, 0.f));
	}

	_vertices.resize(3*_size*_size);
	_coord	 .resize(2*_size*_size);
	_normals .resize(3*_size*_size);
}

void Heightmap::getMapInfo() {
	size_t size1 = _size-1;
	double step =  CHUNK_SIZE / (double) size1;

  Center* tmpRegions[_size][_size];

  double x, x1, x2, x3;
  double y, y1, y2, y3;
  x = _chunkPos.x * CHUNK_SIZE;

  for (size_t i = 0 ; i < _size ; i++) {
    y = _chunkPos.y * CHUNK_SIZE;

    for (size_t j = 0 ; j < _size ; j++) {
			Center* currentCenter = _map.getClosestCenter(sf::Vector2<double>(x, y));
      tmpRegions[i][j] = currentCenter;

			bool tryNextAdjacentCenter = true;
			size_t currentAdjacentCenter = 0;
			do {
				x1 = tmpRegions[i][j]->x;
	      y1 = tmpRegions[i][j]->y;

	      for (unsigned int k = 0 ; k < tmpRegions[i][j]->edges.size() ; k++) {
	        if (!tmpRegions[i][j]->edges[k]->mapEdge) {

						x2 = tmpRegions[i][j]->edges[k]->corner0->x;
	          y2 = tmpRegions[i][j]->edges[k]->corner0->y;
	          x3 = tmpRegions[i][j]->edges[k]->corner1->x;
	          y3 = tmpRegions[i][j]->edges[k]->corner1->y;

						// Linear interpolation to get the height
	          double s = ((y2-y3)*(x-x3)+(x3-x2)*(y-y3)) / ((y2-y3)*(x1-x3)+(x3-x2)*(y1-y3));
	          double t = ((y3-y1)*(x-x3)+(x1-x3)*(y-y3)) / ((y2-y3)*(x1-x3)+(x3-x2)*(y1-y3));

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
      y += step;
    }

    x += step;
  }


  for (size_t i = 0 ; i < size1 ; i++) {
    for (size_t j = 0 ; j < size1 ; j++) {
			// Set the indices for the two triangles that will constitute a square which
			// top left corner will be of position (i,j)

      std::vector<GLuint>& currentIndices = _indices[tmpRegions[i][j]->biome];

      currentIndices.push_back( 		i*_size + j);
      currentIndices.push_back( (i+1)*_size + j);
      currentIndices.push_back( 		i*_size + j+1);

			currentIndices.push_back( 		i*_size + j+1);
      currentIndices.push_back( (i+1)*_size + j);
      currentIndices.push_back( (i+1)*_size + j+1);
    }
  }
}

void Heightmap::fillBufferData() {
	size_t size1 = _size-1;
	double step =  CHUNK_SIZE / (double) size1;

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

void Heightmap::generateBuffers() {
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);

	size_t bufferSizeVertices = _vertices.size()*sizeof _vertices[0];
	size_t bufferSizeCoord		= _coord.size()*sizeof _coord[0];
	size_t bufferSizeNormals	= _normals.size()*sizeof _normals[0];

  glBufferData(GL_ARRAY_BUFFER, bufferSizeVertices + bufferSizeCoord + bufferSizeNormals, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, bufferSizeVertices, &_vertices[0]);
	glBufferSubData(GL_ARRAY_BUFFER, bufferSizeVertices, bufferSizeCoord, &_coord[0]);
  glBufferSubData(GL_ARRAY_BUFFER, bufferSizeVertices + bufferSizeCoord, bufferSizeNormals, &_normals[0]);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*(_size-1)*(_size-1) * sizeof(GLuint), NULL, GL_STATIC_DRAW);

  int cursor = 0;
  for (auto it = _indices.begin(); it != _indices.end() ; ++it ) {
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, cursor, it->second.size() * sizeof(GLuint), &(it->second[0]));
    cursor += it->second.size() * sizeof(GLuint);
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Heightmap::computeLowestsHighests() {
	size_t size1 = _size-1;
	double step =  CHUNK_SIZE / (double) size1;
	float minH = HEIGHT_FACTOR;
	float maxH = 0.;
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
	maxH = 0.;
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
	maxH = 0.;
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
	maxH = 0.;
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

void Heightmap::generate(std::vector<Constraint> constraints) {
  size_t size1 = _size-1;
	double step =  CHUNK_SIZE / (double) size1;

  getMapInfo();

  fillBufferData();

	generateBuffers();

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
  for (auto it = _indices.begin(); it != _indices.end() ; ++it ) {
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
  float alpha = cam.getFov() * cam.getRatio() / 2.;

  // Bottom of the view
  sf::Vector3f norm = vu::carthesian(1., theta, phi + 90. - cam.getFov() / 2.);
  sf::Vector3f pos = cam.getPos();

  if (compareToPoints(pos,norm,_corners) == 1 &&
			compareToPoints(pos,norm,_highests) == 1) {
    _visible = false;
    return;
  }

  // Top
  norm = vu::carthesian(1., theta, phi + 90. + cam.getFov() / 2.);
  norm *= -1.f;

  if (compareToPoints(pos,norm,_corners) == 1 &&
			compareToPoints(pos,norm,_lowests) == 1) {
    _visible = false;
    return;
  }

  // Right
  norm = vu::carthesian(1., theta + 90.f, 90.f);
  vu::Mat3f rot;
  rot.rotation(vu::carthesian(1., theta + 180., 90.f - phi), - alpha);
  norm = rot.multiply(norm);

  if (compareToPoints(pos,norm,_corners) == 1) {
    _visible = false;
    return;
  }

  // Left
  norm = vu::carthesian(1., theta - 90.f, 90.f);
  rot.rotation(vu::carthesian(1., theta + 180., 90.f - phi), alpha);
  norm = rot.multiply(norm);

  if (compareToPoints(pos,norm,_corners) == 1) {
    _visible = false;
    return;
  }

  _visible = true;
}

Constraint Heightmap::getConstraint(sf::Vector2i fromChunkPos) const {
  Constraint c;
  c.vertices = std::vector<sf::Vector3f>(_size);

  if(fromChunkPos == sf::Vector2i(_chunkPos.x+1, _chunkPos.y)) {
    for (size_t i = 0 ; i < _size ; i++)
      c.vertices[i] = sf::Vector3f(CHUNK_SIZE, i * CHUNK_SIZE / (_size-1), _heights[_size-1][i]);

    c.type = XN;
  }

  else if(fromChunkPos == sf::Vector2i(_chunkPos.x-1, _chunkPos.y)) {
    for (size_t i = 0 ; i < _size ; i++)
      c.vertices[i] = sf::Vector3f(0.f, i * CHUNK_SIZE / (_size-1), _heights[0][i]);

    c.type = XP;
  }

  else if(fromChunkPos == sf::Vector2i(_chunkPos.x, _chunkPos.y+1)) {
    for (size_t i = 0 ; i < _size ; i++)
      c.vertices[i] = sf::Vector3f(i * CHUNK_SIZE / (_size-1), CHUNK_SIZE, _heights[i][_size-1]);

    c.type = YN;
  }

  else if(fromChunkPos == sf::Vector2i(_chunkPos.x, _chunkPos.y-1)) {
    for (size_t i = 0 ; i < _size ; i++)
      c.vertices[i] = sf::Vector3f( i * CHUNK_SIZE / (_size-1), 0.f, _heights[i][0]);

    c.type = YP;
  }

  else
    c.type = NONE;

  return c;
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
