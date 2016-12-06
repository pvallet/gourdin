#include "terrainGeometry.h"

#include <iostream>
#include <unordered_set>
#include <utility>

TerrainGeometry::TerrainGeometry() {
  std::vector<std::list<Triangle*> > initializer(SUBDIV_LVL*SUBDIV_LVL);
  _trianglesInSubChunk.resize(NB_CHUNKS*NB_CHUNKS, initializer);
}

void TerrainGeometry::addTriangle(std::array<sf::Vector3f,3> p, std::array<Biome,4> biomes) {

  // Add the triangle to the list of all triangles

  Triangle newTriangle;
  newTriangle.biome = biomes[3];
  std::copy(biomes.begin(), biomes.begin()+3, newTriangle.neighbourBiomes.begin());
  sf::Vector3f normal = vu::cross(p[1]-p[0],p[2]-p[0]);
  newTriangle.normal = normal /= vu::norm(normal);

  // If the order of the vertices is wrong, fix it
  if (vu::dot(normal, sf::Vector3f(0,0,1)) < 0) {
    std::swap(p[1],p[2]);
    newTriangle.normal = - 1.f * newTriangle.normal;
  }
  _triangles.push_back(newTriangle);

  // Add the vertices to the list of all vertices

  for (size_t i = 0; i < 3; i++) {
    if (_vertices.find(p[i]) == _vertices.end()) {
      Vertex vertex;
      vertex.pos = p[i];
      vertex.distToBiomeEdge = -1;
      vertex.belongsToTris.push_back(&_triangles.back());

      _vertices.insert(std::pair<sf::Vector3f, Vertex>(p[i], vertex));
    }

    else
      _vertices.at(p[i]).belongsToTris.push_back(&_triangles.back());

    _triangles.back().vertices[i] = &_vertices.at(p[i]);
  }

  // Sort the triangle to place it in the adequate subchunks according to its bounding box

  sf::Vector2f minAbsCoord, maxAbsCoord;
  minAbsCoord.x = std::min(p[0].x, std::min(p[1].x, p[2].x));
  minAbsCoord.y = std::min(p[0].y, std::min(p[1].y, p[2].y));
  maxAbsCoord.x = std::max(p[0].x, std::max(p[1].x, p[2].x)); if (maxAbsCoord.x == MAX_COORD) maxAbsCoord.x--;
  maxAbsCoord.y = std::max(p[0].y, std::max(p[1].y, p[2].y)); if (maxAbsCoord.y == MAX_COORD) maxAbsCoord.y--;

  std::array<sf::Vector2u, 2> minChunkCoords = getSubChunkInfo(minAbsCoord);
  std::array<sf::Vector2u, 2> maxChunkCoords = getSubChunkInfo(maxAbsCoord);

  for (size_t i = minChunkCoords[0].x; i < maxChunkCoords[0].x+1; i++) {
  for (size_t j = minChunkCoords[0].y; j < maxChunkCoords[0].y+1; j++) {
    for (size_t k = (i == minChunkCoords[0].x ? minChunkCoords[1].x : 0);
                k < (i == maxChunkCoords[0].x ? maxChunkCoords[1].x + 1 : SUBDIV_LVL); k++) {
    for (size_t l = (j == minChunkCoords[0].y ? minChunkCoords[1].y : 0);
                l < (j == maxChunkCoords[0].y ? maxChunkCoords[1].y + 1 : SUBDIV_LVL); l++) {


      _trianglesInSubChunk[i*NB_CHUNKS + j][k*SUBDIV_LVL + l].push_back(&_triangles.back());
    }
    }
  }
  }
}

std::array<size_t,3> TerrainGeometry::sortIndices(sf::Vector3f refPoint, const Triangle& tri) const {
  std::array<size_t,3> res;

  if (refPoint == tri.vertices[0]->pos) {
    res[0] = 0;
    res[1] = 1;
    res[2] = 2;
  }

  else if (refPoint == tri.vertices[1]->pos) {
    res[0] = 1;
    res[1] = 2;
    res[2] = 0;
  }

  else if (refPoint == tri.vertices[2]->pos) {
    res[0] = 2;
    res[1] = 0;
    res[2] = 1;
  }

  else
    std::cerr << "Error in TerrainGeometry::sortIndices, no match for refPoint in triangle" << std::endl;

  return res;
}

void TerrainGeometry::computeNormals() {
  for (auto p = _vertices.begin(); p != _vertices.end(); p++) {
    sf::Vector3f normal(0.f,0.f,0.f);

    float totalWeight = 0;

    for (auto t = p->second.belongsToTris.begin(); t != p->second.belongsToTris.end(); t++) {
      float weight;

      std::array<size_t,3> srt = sortIndices(p->first,**t);

      weight = vu::absoluteAngle((*t)->vertices[srt[1]]->pos-(*t)->vertices[srt[0]]->pos,
                                 (*t)->vertices[srt[2]]->pos-(*t)->vertices[srt[0]]->pos);

      normal += weight * (*t)->normal;
      totalWeight += weight;
    }

    normal /= totalWeight;

    p->second.normal = normal;
  }
}

void TerrainGeometry::computeDistancesToBiomeEdges() {
  std::unordered_set<sf::Vector3f, vertHashFunc> leftToProcess;

  // Compute borders

  for (auto tri = _triangles.begin(); tri != _triangles.end(); tri++) {
    std::vector<bool> addToList(3,true);

    for (size_t i = 0; i < 3; i++) {
      if (tri->neighbourBiomes[i] != tri->biome) {
        addToList[i] = false;
        addToList[(i+1)%3] = false;
        tri->vertices[i]->distToBiomeEdge = 0;
        tri->vertices[(i+1)%3]->distToBiomeEdge = 0;
      }
    }

    for (size_t i = 0; i < 3; i++) {
      if (addToList[i])
        leftToProcess.insert(tri->vertices[i]->pos);
    }
  }

  // Compute distances iteratively

  while (leftToProcess.size() != 0) {
    std::unordered_set<sf::Vector3f, vertHashFunc> leftToProcessNxt;

    for (auto pt = leftToProcess.begin(); pt != leftToProcess.end(); pt++) {

      float minDist = MAX_COORD;

      for (auto tri = _vertices.at(*pt).belongsToTris.begin();
               tri != _vertices.at(*pt).belongsToTris.end(); tri++) {

        std::array<size_t, 3> srt = sortIndices(*pt, **tri);
        if ((*tri)->vertices[srt[1]]->distToBiomeEdge != -1 &&
            (*tri)->vertices[srt[2]]->distToBiomeEdge != -1) {

          // A bit of magic, see the report
          sf::Vector2f a,b;
          float d, baseD;

          if ((*tri)->vertices[srt[1]]->distToBiomeEdge >
              (*tri)->vertices[srt[2]]->distToBiomeEdge) {

            b.x = (*tri)->vertices[srt[0]]->pos.x-(*tri)->vertices[srt[1]]->pos.x;
            b.y = (*tri)->vertices[srt[0]]->pos.y-(*tri)->vertices[srt[1]]->pos.y;
            d = (*tri)->vertices[srt[1]]->distToBiomeEdge -
                (*tri)->vertices[srt[2]]->distToBiomeEdge;

            baseD = (*tri)->vertices[srt[2]]->distToBiomeEdge;
          }

          else {
            b.x = (*tri)->vertices[srt[0]]->pos.x-(*tri)->vertices[srt[2]]->pos.x;
            b.y = (*tri)->vertices[srt[0]]->pos.y-(*tri)->vertices[srt[2]]->pos.y;
            d = (*tri)->vertices[srt[2]]->distToBiomeEdge -
                (*tri)->vertices[srt[1]]->distToBiomeEdge;

            baseD = (*tri)->vertices[srt[1]]->distToBiomeEdge;
          }

          a.x = (*tri)->vertices[srt[2]]->pos.x-(*tri)->vertices[srt[1]]->pos.x;
          a.y = (*tri)->vertices[srt[2]]->pos.y-(*tri)->vertices[srt[1]]->pos.y;

          float alpha = abs(vu::angle(a,b)) * M_PI / 180;

          minDist = std::min(minDist,
           vu::norm(b) * (float) cos(M_PI-alpha-acos(d/vu::norm(a))) + d + baseD);
        }
      }

      if (minDist < MAX_COORD)
        _vertices.at(*pt).distToBiomeEdge = minDist;

      else
        leftToProcessNxt.insert(*pt);
    }

    leftToProcess = leftToProcessNxt;
  }
}

bool TerrainGeometry::isOcean(size_t x, size_t y) const {
  for (size_t i = 0; i < _trianglesInSubChunk[x*NB_CHUNKS + y].size(); i++) {
    if (_trianglesInSubChunk[x*NB_CHUNKS + y][i].size() == 0)
      return true;
  }

  return false;
}

std::array<sf::Vector2u, 2> TerrainGeometry::getSubChunkInfo(sf::Vector2f pos) const {
  std::array<sf::Vector2u, 2> res;

  res[0].x = pos.x / CHUNK_SIZE;
  res[0].y = pos.y / CHUNK_SIZE;
  res[1].x = (pos.x - res[0].x * CHUNK_SIZE) / (CHUNK_SIZE / SUBDIV_LVL);
  res[1].y = (pos.y - res[0].y * CHUNK_SIZE) / (CHUNK_SIZE / SUBDIV_LVL);

  return res;
}

std::list<Triangle*> TerrainGeometry::getTrianglesInChunk(size_t x, size_t y) const {
  std::list<Triangle*> res;

  for (size_t i = 0; i < _trianglesInSubChunk[x*NB_CHUNKS + y].size(); i++) {
    res.insert(res.end(), _trianglesInSubChunk[x*NB_CHUNKS + y][i].begin(),
                          _trianglesInSubChunk[x*NB_CHUNKS + y][i].end());
  }

  res.sort(compTri());
  res.unique(equalTri());

  return res;
}

std::list<Triangle*> TerrainGeometry::getTrianglesNearPos(sf::Vector2f pos) const {
  std::array<sf::Vector2u, 2> intCoord = getSubChunkInfo(pos);
  return _trianglesInSubChunk[intCoord[0].x*NB_CHUNKS  + intCoord[0].y]
                             [intCoord[1].x*SUBDIV_LVL + intCoord[1].y];
}

Triangle* TerrainGeometry::getTriangleContaining(sf::Vector2f pos) const {
  std::array<sf::Vector2u, 2> intCoord = getSubChunkInfo(pos);
  std::list<Triangle*> toTest =
    _trianglesInSubChunk[intCoord[0].x*NB_CHUNKS  + intCoord[0].y]
                        [intCoord[1].x*SUBDIV_LVL + intCoord[1].y];

  for (auto tri = toTest.begin(); tri != toTest.end(); tri++) {
    float x[3]; float y[3];

    for (size_t i = 0; i < 3; i++) {
      x[i] = (*tri)->vertices[i]->pos.x;
      y[i] = (*tri)->vertices[i]->pos.y;
    }

    float s = ((y[1]-y[2])*(pos.x-x[2])+(x[2]-x[1])*(pos.y-y[2])) /
              ((y[1]-y[2])*(x[0]-x[2])+(x[2]-x[1])*(y[0]-y[2]));

    float t = ((y[2]-y[0])*(pos.x-x[2])+(x[0]-x[2])*(pos.y-y[2])) /
              ((y[1]-y[2])*(x[0]-x[2])+(x[2]-x[1])*(y[0]-y[2]));

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1 && s + t <= 1) {
      return *tri;
    }
  }

  return nullptr;
}
