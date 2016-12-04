#include "terrainGeometry.h"


void TerrainGeometry::addTriangle(std::array<sf::Vector3f,3> p, Biome biome) {

  // Add the triangle to the list of all triangles

  Triangle newTriangle;
  newTriangle.points = p;
  newTriangle.biome = biome;
  sf::Vector3f normal = vu::cross(p[1]-p[0],p[2]-p[0]);
  newTriangle.normal = normal /= vu::norm(normal);
  _triangles.push_back(newTriangle);

  // Add the points to the list of all points

  for (size_t i = 0; i < 3; i++) {
    if (_points.find(p[i]) == _points.end()) {
      Point point;
      point.pos = p[i];
      point.belongsToTris.push_back(&_triangles.back());

      _points.insert(std::pair<sf::Vector3f, Point>(p[i], point));
    }

    else
      _points.at(p[i]).belongsToTris.push_back(&_triangles.back());
  }

  // Sort the triangle to place it in the adequate subchunks according to its bounding box

  sf::Vector2f minAbsCoord, maxAbsCoord;
  minAbsCoord.x = std::min(p[0].x, std::min(p[1].x, p[2].x));
  minAbsCoord.y = std::min(p[0].y, std::min(p[1].y, p[2].y));
  maxAbsCoord.x = std::max(p[0].x, std::max(p[1].x, p[2].x)); if (maxAbsCoord.x == MAX_COORD) maxAbsCoord.x--;
  maxAbsCoord.y = std::max(p[0].y, std::max(p[1].y, p[2].y)); if (maxAbsCoord.y == MAX_COORD) maxAbsCoord.y--;

  std::array<sf::Vector2u, 2> minChunkCoords = getSubChunkInfo(minAbsCoord);
  std::array<sf::Vector2u, 2> maxChunkCoords = getSubChunkInfo(maxAbsCoord);

  for (size_t i = minChunkCoords[0].x; i < maxChunkCoords[0].x; i++) {
  for (size_t j = minChunkCoords[0].y; j < maxChunkCoords[0].y; j++) {
    for (size_t k = minChunkCoords[1].x; k < maxChunkCoords[1].x; k++) {
    for (size_t l = minChunkCoords[1].y; l < maxChunkCoords[1].y; l++) {

      _trianglesInSubChunk[i*NB_CHUNKS + j][k*SUBDIV_LVL + l].push_back(&_triangles.back());
    }
    }
  }
  }
}

void TerrainGeometry::computeNormals() {
  for (auto p = _points.begin(); p != _points.end(); p++) {
    sf::Vector3f normal(0.f,0.f,0.f);

    for (auto t = p->second.belongsToTris.begin(); t != p->second.belongsToTris.end(); t++) {
      float weight;
      if (p->first == (*t)->points[0])
        weight = vu::absoluteAngle((*t)->points[1]-(*t)->points[0], (*t)->points[2]-(*t)->points[0]);
      else if (p->first == (*t)->points[1])
        weight = vu::absoluteAngle((*t)->points[0]-(*t)->points[1], (*t)->points[2]-(*t)->points[1]);
      else
        weight = vu::absoluteAngle((*t)->points[1]-(*t)->points[2], (*t)->points[0]-(*t)->points[2]);

      normal += weight * (*t)->normal;
    }

    normal /= 360.f;

    p->second.normal = normal;
  }
}

std::array<sf::Vector2u, 2> TerrainGeometry::getSubChunkInfo(sf::Vector2f pos) {
  std::array<sf::Vector2u, 2> res;

  res[0].x = pos.x / CHUNK_SIZE;
  res[0].y = pos.y / CHUNK_SIZE;
  res[1].x = (pos.x - res[0].x * CHUNK_SIZE) / (int)(CHUNK_SIZE / SUBDIV_LVL);
  res[1].y = (pos.y - res[0].y * CHUNK_SIZE) / (int)(CHUNK_SIZE / SUBDIV_LVL);

  return res;
}

std::list<Triangle*> TerrainGeometry::getTrianglesNearPos(sf::Vector2f pos) {
  std::array<sf::Vector2u, 2> intCoord = getSubChunkInfo(pos);
  return _trianglesInSubChunk[intCoord[0].x*NB_CHUNKS  + intCoord[0].y]
                             [intCoord[1].x*SUBDIV_LVL + intCoord[1].y];
}
