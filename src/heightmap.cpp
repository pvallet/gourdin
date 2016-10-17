#include "heightmap.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <omp.h>

#include "perlin.h"
#include "vecUtils.h"

#define DEFAULT_MAP_SIZE 33 // +1 to join with other chunks
#define BUFFER_OFFSET(a) ((char*)NULL + (a))

#define SMOOTH_RANGE 20. // Percentage


Heightmap::Heightmap(sf::Vector2i chunkPosition, int seed, TerrainTexManager* texManager, Map* map) :
	Chunk(chunkPosition),
	_size(DEFAULT_MAP_SIZE),
  _seed(seed),
  _texManager(texManager),
  _map(map) {

  for (unsigned int i = 0 ; i < DEFAULT_MAP_SIZE ; i++) {
		_heights.push_back(std::vector<float>(DEFAULT_MAP_SIZE, 0.0f));
	}

	_vertices   = new float [3*4*(_size-1)*(_size-1)];
  _normals    = new float [3*4*(_size-1)*(_size-1)];
  _coord      = new float [2*4*(_size-1)*(_size-1)];
}

void Heightmap::generate(std::vector<Constraint> constraints) {
  /*Perlin perlin(_seed, _size);

	for (int i = 0 ; i < _size ; i++) {
		for (int j = 0 ; j < _size ; j++) {
			_heights[i][j] = perlin.getValue(i, j) * HEIGHT_FACTOR;
		}
	}*/

  int size1 = _size-1;

  double step =  CHUNK_SIZE / (double) size1;

  // Get info from the map
  Center* tmpRegions[_size][_size];

  double x, x1, x2, x3;
  double y, y1, y2, y3;
  x = _chunkPos.x * CHUNK_SIZE;

  for (int i = 0 ; i < _size ; i++) {
    y = _chunkPos.y * CHUNK_SIZE;

    for (int j = 0 ; j < _size ; j++) {
      tmpRegions[i][j] = _map->getClosestCenter(sf::Vector2<double>(x, y));
      _regions[tmpRegions[i][j]->id].cursor = 0;

      x1 = tmpRegions[i][j]->x;
      y1 = tmpRegions[i][j]->y;

      _heights[i][j] = 0.;

      int chosenTriangle = 1000;
      std::ostringstream tamere;
      tamere << "chosen cell " << tmpRegions[i][j]->id << " Center " << tmpRegions[i][j]->x << " " << tmpRegions[i][j]->y <<
              " - Point Coords - " << x << " " << y << " " << vu::norm(sf::Vector2<double>(x, y) - sf::Vector2<double>(x1, y1)) << std::endl;

			tamere << "Other cells dists ";

      for (unsigned int k = 0 ; k < tmpRegions[i][j]->centers.size() ; k++){
          tamere << vu::norm(sf::Vector2<double>(x, y) - sf::Vector2<double>(tmpRegions[i][j]->centers[k]->x, tmpRegions[i][j]->centers[k]->y)) << " ";
      }

			tamere << std::endl;

      // Linear interpolation to get the height
      double s, t;
      bool testPassed = false;
      double minS = 1000;
      double minT = 1000;
      int minK = 0;
      for (unsigned int k = 0 ; k < tmpRegions[i][j]->edges.size() ; k++) {
        if (!tmpRegions[i][j]->edges[k]->mapEdge) {

					x2 = tmpRegions[i][j]->edges[k]->corner0->x;
          y2 = tmpRegions[i][j]->edges[k]->corner0->y;
          x3 = tmpRegions[i][j]->edges[k]->corner1->x;
          y3 = tmpRegions[i][j]->edges[k]->corner1->y;

          s = ((y2-y3)*(x-x3)+(x3-x2)*(y-y3)) / ((y2-y3)*(x1-x3)+(x3-x2)*(y1-y3));
          t = ((y3-y1)*(x-x3)+(x1-x3)*(y-y3)) / ((y2-y3)*(x1-x3)+(x3-x2)*(y1-y3));

          tamere << "Triangle coords " << tmpRegions[i][j]->edges[k]->x << " " << tmpRegions[i][j]->edges[k]->y << " - " <<
                  x2 << " " << y2 << " - " << x3 << " " << y3 << std::endl << "Barycentric coordinates " << s << " " << t << std::endl;

          if (s >= 0 && s <= 1 && t >= 0 && t <= 1 && s + t <= 1) {
            _heights[i][j] = s       * tmpRegions[i][j]->elevation +
                            t       * tmpRegions[i][j]->edges[k]->corner0->elevation +
                            (1-s-t) * tmpRegions[i][j]->edges[k]->corner1->elevation;
            if (_heights[i][j] < 0)
              	_heights[i][j] = 0;
            // _heights[i][j] *=  sqrt(_heights[i][j]);
            _heights[i][j] *=  HEIGHT_FACTOR;

            chosenTriangle = k;

						tamere << "kestufoula" << std::endl;

						testPassed = true;
          }

          else if (s < minS) {
            minS = s;
            minT = t;
            minK = k;
          }
        }
			}

			// tamere << std::endl;
			//
			// x1 = tmpRegions[i][j]->centers[tmpRegions[i][j]->centers.size()-2]->x;
      // y1 = tmpRegions[i][j]->centers[tmpRegions[i][j]->centers.size()-2]->y;
			//
			// tamere << "RECHOSEN CELL" << tmpRegions[i][j]->id << " Center " << tmpRegions[i][j]->centers[tmpRegions[i][j]->centers.size()-2]->x << " " << tmpRegions[i][j]->centers[tmpRegions[i][j]->centers.size()-2]->y <<
      //         " - Point Coords - " << x << " " << y << " " << vu::norm(sf::Vector2<double>(x, y) - sf::Vector2<double>(x1, y1)) << std::endl;
			//
			// tamere << "Other cells dists ";
			//
      // for (unsigned int k = 0 ; k < tmpRegions[i][j]->centers.size() ; k++){
      //     tamere << vu::norm(sf::Vector2<double>(x, y) - sf::Vector2<double>(tmpRegions[i][j]->centers[tmpRegions[i][j]->centers.size()-2]->centers[k]->x, tmpRegions[i][j]->centers[tmpRegions[i][j]->centers.size()-2]->centers[k]->y)) << " ";
      // }
			//
			// tamere << std::endl;
			//
			// for (unsigned int k = 0 ; k < tmpRegions[i][j]->centers[tmpRegions[i][j]->centers.size()-2]->edges.size() ; k++) {
      //   if (!tmpRegions[i][j]->centers[tmpRegions[i][j]->centers.size()-2]->edges[k]->mapEdge) {
			//
			// 		x2 = tmpRegions[i][j]->centers[tmpRegions[i][j]->centers.size()-2]->edges[k]->corner0->x;
      //     y2 = tmpRegions[i][j]->centers[tmpRegions[i][j]->centers.size()-2]->edges[k]->corner0->y;
      //     x3 = tmpRegions[i][j]->centers[tmpRegions[i][j]->centers.size()-2]->edges[k]->corner1->x;
      //     y3 = tmpRegions[i][j]->centers[tmpRegions[i][j]->centers.size()-2]->edges[k]->corner1->y;
			//
      //     s = ((y2-y3)*(x-x3)+(x3-x2)*(y-y3)) / ((y2-y3)*(x1-x3)+(x3-x2)*(y1-y3));
      //     t = ((y3-y1)*(x-x3)+(x1-x3)*(y-y3)) / ((y2-y3)*(x1-x3)+(x3-x2)*(y1-y3));
			//
      //     tamere << "Triangle coords 2 " << tmpRegions[i][j]->centers[tmpRegions[i][j]->centers.size()-2]->edges[k]->x << " " << tmpRegions[i][j]->centers[tmpRegions[i][j]->centers.size()-2]->edges[k]->y << " - " <<
      //             x2 << " " << y2 << " - " << x3 << " " << y3 << std::endl << "Barycentric coordinates " << s << " " << t << std::endl;
      //   }
      // }

      // if (!testPassed) {
      //   _heights[i][j] = minS * tmpRegions[i][j]->elevation +
      //                   minT * tmpRegions[i][j]->edges[minK]->corner0->elevation +
      //                   (1-minS-minT) * tmpRegions[i][j]->edges[minK]->corner1->elevation;
      //   if (_heights[i][j] < 0)
      //       _heights[i][j] = 0;
      //   _heights[i][j] *=  sqrt(_heights[i][j]);
      //   _heights[i][j] *=  HEIGHT_FACTOR;
      // }

      // if (chosenTriangle == 1000 && tmpRegions[i][j]->biome != OCEAN && tmpRegions[i][j]->id == 1050)
      //     std::cout << tamere.str() << std::endl << std::endl;

      if (i != size1 && j != size1) // We must not put the last indices in the ibo
        _regions[tmpRegions[i][j]->id].count++;

      y += step;
    }

    x += step;
  }

  for (auto it = _regions.begin(); it != _regions.end() ; ++it ) {
    it->second.indices = new GLuint[6*it->second.count];
  }

  Region* region;
  for (int i = 0 ; i < size1 ; i++) {
    for (int j = 0 ; j < size1 ; j++) {
      region = &_regions[tmpRegions[i][j]->id];
      region->indices[region->cursor]     =               + i*size1 + j;
      region->indices[region->cursor + 1] =   size1*size1 + i*size1 + j;
      region->indices[region->cursor + 2] = 2*size1*size1 + i*size1 + j;

      region->indices[region->cursor + 3] =   size1*size1 + i*size1 + j;
      region->indices[region->cursor + 4] = 3*size1*size1 + i*size1 + j;
      region->indices[region->cursor + 5] = 2*size1*size1 + i*size1 + j;

      region->cursor += 6;
    }
  }

  sf::Vector3f normalsTmp[_size][_size];

  // Compute normals

  sf::Vector3f normal;
  sf::Vector3f neighbourVec[4];

  for (int i = 1 ; i < size1 ; i++) {
    for (int j = 1 ; j < size1 ; j++) {
      neighbourVec[0] = sf::Vector3f(step,  _heights[i-1][j] - _heights[i][j], 0.f);
      neighbourVec[1] = sf::Vector3f(0.f,   _heights[i][j-1] - _heights[i][j], step);
      neighbourVec[2] = sf::Vector3f(-step, _heights[i+1][j] - _heights[i][j], 0.f);
      neighbourVec[3] = sf::Vector3f(0.f,   _heights[i][j+1] - _heights[i][j], -step);

      normal = vu::cross(neighbourVec[0], neighbourVec[1]) +
               vu::cross(neighbourVec[1], neighbourVec[2]) +
               vu::cross(neighbourVec[2], neighbourVec[3]) +
               vu::cross(neighbourVec[3], neighbourVec[0]);

      normal /= -1.f * vu::norm(normal); // Because our coordinate system is indirect

      normalsTmp[i][j] = normal;
    }
  }

  for (int i = 1 ; i < size1 ; i++) {
    normalsTmp[0][i]     = normalsTmp[1][i];
    normalsTmp[size1][i] = normalsTmp[_size-2][i];
    normalsTmp[i][0]     = normalsTmp[i][1];
    normalsTmp[i][size1] = normalsTmp[i][_size-2];
  }

  normalsTmp[0][0] = normalsTmp[0][1];
  normalsTmp[0][size1] = normalsTmp[1][size1];
  normalsTmp[size1][0] = normalsTmp[size1][1];
  normalsTmp[size1][size1] = normalsTmp[size1][_size-2];

  // Top left corner
  #pragma omp parallel for
	for (int i = 0 ; i < size1 ; i++) {
	for (int j = 0 ; j < size1 ; j++) {
      _coord[2*i*size1 + 2*j]     = (float) i / (float) size1 * TEX_FACTOR;
      _coord[2*i*size1 + 2*j + 1] = (float) j / (float) size1 * TEX_FACTOR;

      _vertices[3*i*size1 + 3*j] 	  = i * step;
      _vertices[3*i*size1 + 3*j + 1] = _heights[i][j];
      _vertices[3*i*size1 + 3*j + 2] = j * step;

      _normals[3*i*size1 + 3*j]     = normalsTmp[i][j].x;
      _normals[3*i*size1 + 3*j + 1] = normalsTmp[i][j].y;
      _normals[3*i*size1 + 3*j + 2] = normalsTmp[i][j].z;
    }
  }

  // Top right corner
  #pragma omp parallel for
  for (int i = 0 ; i < size1 ; i++) {
    for (int j = 0 ; j < size1 ; j++) {

      _coord[2*size1*size1 + 2*i*size1 + 2*j]     = (float) i / (float) size1 * TEX_FACTOR;
      _coord[2*size1*size1 + 2*i*size1 + 2*j + 1] = (float) (j+1) / (float) size1 * TEX_FACTOR;

      _vertices[3*size1*size1 + 3*i*size1 + 3*j]     = i * step;
      _vertices[3*size1*size1 + 3*i*size1 + 3*j + 1] = _heights[i][j+1];
      _vertices[3*size1*size1 + 3*i*size1 + 3*j + 2] = (j+1) * step;

      _normals[3*size1*size1 + 3*i*size1 + 3*j]     = normalsTmp[i][j+1].x;
      _normals[3*size1*size1 + 3*i*size1 + 3*j + 1] = normalsTmp[i][j+1].y;
      _normals[3*size1*size1 + 3*i*size1 + 3*j + 2] = normalsTmp[i][j+1].z;
    }
  }

  // Bottom left corner
  #pragma omp parallel for
  for (int i = 0 ; i < size1 ; i++) {
    for (int j = 0 ; j < size1 ; j++) {

      _coord[4*size1*size1 + 2*i*size1 + 2*j]     = (float) (i+1) / (float) size1 * TEX_FACTOR;
      _coord[4*size1*size1 + 2*i*size1 + 2*j + 1] = (float) j / (float) size1 * TEX_FACTOR;

      _vertices[6*size1*size1 + 3*i*size1 + 3*j]     = (i+1) * step;
      _vertices[6*size1*size1 + 3*i*size1 + 3*j + 1] = _heights[i+1][j];
      _vertices[6*size1*size1 + 3*i*size1 + 3*j + 2] = j * step;

      _normals[6*size1*size1 + 3*i*size1 + 3*j]     = normalsTmp[i+1][j].x;
      _normals[6*size1*size1 + 3*i*size1 + 3*j + 1] = normalsTmp[i+1][j].y;
      _normals[6*size1*size1 + 3*i*size1 + 3*j + 2] = normalsTmp[i+1][j].z;
    }
  }

  // Bottom right corner
  #pragma omp parallel for
  for (int i = 0 ; i < size1 ; i++) {
    for (int j = 0 ; j < size1 ; j++) {

      _coord[6*size1*size1 + 2*i*size1 + 2*j]     = (float) (i+1) / (float) size1 * TEX_FACTOR;
      _coord[6*size1*size1 + 2*i*size1 + 2*j + 1] = (float) (j+1) / (float) size1 * TEX_FACTOR;

      _vertices[9*size1*size1 + 3*i*size1 + 3*j]     = (i+1) * step;
      _vertices[9*size1*size1 + 3*i*size1 + 3*j + 1] = _heights[i+1][j+1];
      _vertices[9*size1*size1 + 3*i*size1 + 3*j + 2] = (j+1) * step;

      _normals[9*size1*size1 + 3*i*size1 + 3*j]     = normalsTmp[i+1][j+1].x;
      _normals[9*size1*size1 + 3*i*size1 + 3*j + 1] = normalsTmp[i+1][j+1].y;
      _normals[9*size1*size1 + 3*i*size1 + 3*j + 2] = normalsTmp[i+1][j+1].z;
    }
  }

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);

  glBufferData(GL_ARRAY_BUFFER, 2*(3*4*(_size-1)*(_size-1)*sizeof *_vertices) + (2*4*(_size-1)*(_size-1)*sizeof *_coord), NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, (3*4*(_size-1)*(_size-1)*sizeof *_vertices), _vertices);
  glBufferSubData(GL_ARRAY_BUFFER, (3*4*(_size-1)*(_size-1)*sizeof *_vertices), (3*4*(_size-1)*(_size-1)*sizeof *_normals), _normals);
  glBufferSubData(GL_ARRAY_BUFFER, 2*(3*4*(_size-1)*(_size-1)*sizeof *_vertices), (2*4*(_size-1)*(_size-1)*sizeof *_coord), _coord);

  glBindBuffer(GL_ARRAY_BUFFER, 0);


  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*(_size-1)*(_size-1) * sizeof(GLuint), NULL, GL_STATIC_DRAW);

  int cursor = 0;
  for (auto it = _regions.begin(); it != _regions.end() ; ++it ) {
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, cursor, 6*it->second.count * sizeof(GLuint), it->second.indices);
    cursor += 6*it->second.count * sizeof(GLuint);
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  _corners[0] = sf::Vector3f(_chunkPos.x * CHUNK_SIZE, _heights[0][0], _chunkPos.y * CHUNK_SIZE);
  _corners[1] = sf::Vector3f(_chunkPos.x * CHUNK_SIZE, _heights[0][size1], (_chunkPos.y+1) * CHUNK_SIZE);
  _corners[2] = sf::Vector3f((_chunkPos.x+1) * CHUNK_SIZE, _heights[size1][size1], (_chunkPos.y+1) * CHUNK_SIZE);
  _corners[3] = sf::Vector3f((_chunkPos.x+1) * CHUNK_SIZE, _heights[size1][0], _chunkPos.y * CHUNK_SIZE);

  float minH = HEIGHT_FACTOR;
  int iMin = 0;
  int iMax = 0;
  float maxH = 0.;

  for (int i = 0 ; i < _size ; i++) {
    if (_heights[0][i] < minH) {
      iMin = i;
      minH = _heights[0][i];
    }

    if (_heights[0][i] > maxH) {
      iMax = i;
      maxH = _heights[0][i];
    }
  }

  _lowests[0]  = sf::Vector3f(_chunkPos.x * CHUNK_SIZE, _heights[0][iMin], _chunkPos.y * CHUNK_SIZE + iMin * step);
  _highests[0] = sf::Vector3f(_chunkPos.x * CHUNK_SIZE, _heights[0][iMax], _chunkPos.y * CHUNK_SIZE + iMax * step);

  for (int i = 0 ; i < _size ; i++) {
    if (_heights[size1][i] < minH) {
      iMin = i;
      minH = _heights[size1][i];
    }

    if (_heights[size1][i] > maxH) {
      iMax = i;
      maxH = _heights[size1][i];
    }
  }

  _lowests[1]  = sf::Vector3f((_chunkPos.x+1) * CHUNK_SIZE, _heights[size1][iMin], _chunkPos.y * CHUNK_SIZE + iMin * step);
  _highests[1] = sf::Vector3f((_chunkPos.x+1) * CHUNK_SIZE, _heights[size1][iMax], _chunkPos.y * CHUNK_SIZE + iMax * step);

  for (int i = 0 ; i < _size ; i++) {
    if (_heights[i][0] < minH) {
      iMin = i;
      minH = _heights[i][0];
    }

    if (_heights[i][0] > maxH) {
      iMax = i;
      maxH = _heights[i][0];
    }
  }

  _lowests[2]  = sf::Vector3f(_chunkPos.x * CHUNK_SIZE + iMin * step, _heights[iMin][0], _chunkPos.y * CHUNK_SIZE);
  _highests[2] = sf::Vector3f(_chunkPos.x * CHUNK_SIZE + iMax * step, _heights[iMax][0], _chunkPos.y * CHUNK_SIZE);

  for (int i = 0 ; i < _size ; i++) {
    if (_heights[i][size1] < minH) {
      iMin = i;
      minH = _heights[i][size1];
    }

    if (_heights[i][size1] > maxH) {
      iMax = i;
      maxH = _heights[i][size1];
    }
  }

  _lowests[3]  = sf::Vector3f(_chunkPos.x * CHUNK_SIZE + iMin * step, _heights[iMin][size1], (_chunkPos.y+1) * CHUNK_SIZE);
  _highests[3] = sf::Vector3f(_chunkPos.x * CHUNK_SIZE + iMax * step, _heights[iMax][size1], (_chunkPos.y+1) * CHUNK_SIZE);

}

void Heightmap::draw() const {
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);

  glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0));
  glNormalPointer(GL_FLOAT, 0, BUFFER_OFFSET(3*4*(_size-1)*(_size-1)*sizeof *_vertices));
  glTexCoordPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(2*3*4*(_size-1)*(_size-1)*sizeof *_vertices));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);


  int cursor = 0;
  for (auto it = _regions.begin(); it != _regions.end() ; ++it ) {
    _texManager->bindTexture(_map->getCenters()[it->first]->biome);

    glDrawElements(GL_TRIANGLES, 6*it->second.count, GL_UNSIGNED_INT, BUFFER_OFFSET(cursor));
    cursor += 6*it->second.count * sizeof(GLuint);

    glBindTexture(GL_TEXTURE_2D, 0);
  }


  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Heightmap::saveToImage() const {
	sf::Uint8* pixels = new sf::Uint8[_size * _size * 4];

	for (int i = 0 ; i < _size ; i++) { // Convert _heights to array of pixels
		for (int j = 0 ; j < _size ; j++) {
			for (int k = 0 ; k < 3 ; k++) {
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

int Heightmap::compareToPoints(sf::Vector3f cam, sf::Vector3f vec, sf::Vector3f* points) const {
    float dots[4];

    for (int i = 0 ; i < 4 ; i++) {
        dots[i] = vu::dot(points[i]-cam, vec);
    }

    if (dots[0] >= 0.f && dots[1] >= 0.f &&
      	dots[2] >= 0.f && dots[3] >= 0.f )
      return 1;

    else if (dots[0] <= 0.f && dots[1] <= 0.f &&
             dots[2] <= 0.f && dots[3] <= 0.f )
      return -1;

    else
      return 0;
}

void Heightmap::calculateFrustum(const Camera* camera) {
    float theta = camera->getTheta();
    float phi   = camera->getPhi();
    float alpha = camera->getFov() * camera->getRatio() / 2;

    // Bottom of the view
    sf::Vector3f norm = vu::carthesian(1., theta, phi + 90. - camera->getFov() / 2.);
    float tmp;
    tmp = norm.y;
    norm.y = norm.z;
    norm.z = tmp;
    sf::Vector3f pos = camera->getPos();

    if (compareToPoints(pos,norm,_corners) == 1 && compareToPoints(pos,norm,_highests) == 1) {
      _visible = false;
      return;
    }

    // Top
    norm = vu::carthesian(1., theta, phi + 90. + camera->getFov() / 2.);
    tmp = norm.y;
    norm.y = norm.z;
    norm.z = tmp;
    norm *= -1.f;

    if (compareToPoints(pos,norm,_corners) == 1 && compareToPoints(pos,norm,_lowests) == 1) {
      _visible = false;
      return;
    }

    // Right
    norm = vu::carthesian(1., theta + 90.f, 90.f);
    vu::Mat3f rot;
    rot.rotation(vu::carthesian(1., theta + 180., 90.f - phi), - alpha);
    norm = rot.multiply(norm);
    tmp = norm.y;
    norm.y = norm.z;
    norm.z = tmp;

    if (compareToPoints(pos,norm,_corners) == 1) {
      _visible = false;
      return;
    }

    // Left
    norm = vu::carthesian(1., theta - 90.f, 90.f);
    rot.rotation(vu::carthesian(1., theta + 180., 90.f - phi), alpha);
    norm = rot.multiply(norm);
    tmp = norm.y;
    norm.y = norm.z;
    norm.z = tmp;

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
    for (int i = 0 ; i < _size ; i++)
      c.vertices[i] = sf::Vector3f(CHUNK_SIZE, _heights[_size-1][i], i * CHUNK_SIZE / (_size-1));

    c.type = XN;
  }

  else if(fromChunkPos == sf::Vector2i(_chunkPos.x-1, _chunkPos.y)) {
    for (int i = 0 ; i < _size ; i++)
      c.vertices[i] = sf::Vector3f(0.f, _heights[0][i], i * CHUNK_SIZE / (_size-1));

    c.type = XP;
  }

  else if(fromChunkPos == sf::Vector2i(_chunkPos.x, _chunkPos.y+1)) {
    for (int i = 0 ; i < _size ; i++)
      c.vertices[i] = sf::Vector3f(i * CHUNK_SIZE / (_size-1), _heights[i][_size-1], CHUNK_SIZE);

    c.type = YN;
  }

  else if(fromChunkPos == sf::Vector2i(_chunkPos.x, _chunkPos.y-1)) {
    for (int i = 0 ; i < _size ; i++)
      c.vertices[i] = sf::Vector3f( i * CHUNK_SIZE / (_size-1), _heights[i][0], 0.f);

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
