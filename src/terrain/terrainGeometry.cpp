#include "terrainGeometry.h"

#include <SDL_log.h>
#include <glm/gtx/vector_angle.hpp>
#include <algorithm>
#include <iostream>
#include <unordered_set>
#include <utility>

// Subdivision of the chunks to store the triangles
#define GRID_SUBDIV 8

#define PERLIN_HEIGHT_FACTOR 2000.f

struct compTriClockwiseOrder {
  compTriClockwiseOrder(glm::vec3 basePoint) {_basePoint = basePoint;}

  bool operator()(const Triangle* lhs, const Triangle* rhs) const {
    std::array<size_t,3> srtL = lhs->sortIndices(_basePoint);
    std::array<size_t,3> srtR = rhs->sortIndices(_basePoint);

    glm::vec2 refVector(0,1);
    // Angle between first edge of the triangle and vec(0,1)
    float lhsAngle = glm::orientedAngle(
      refVector,
      glm::normalize(glm::vec2(lhs->vertices[srtL[1]]->pos.x - lhs->vertices[srtL[0]]->pos.x,
                               lhs->vertices[srtL[1]]->pos.y - lhs->vertices[srtL[0]]->pos.y))
    );

    float rhsAngle = glm::orientedAngle(
      refVector,
      glm::normalize(glm::vec2(rhs->vertices[srtR[1]]->pos.x - rhs->vertices[srtR[0]]->pos.x,
                               rhs->vertices[srtR[1]]->pos.y - rhs->vertices[srtR[0]]->pos.y))
    );

    if (lhsAngle < 0)
      lhsAngle += 2*M_PI;
    if (rhsAngle < 0)
      rhsAngle += 2*M_PI;

    return lhsAngle < rhsAngle;
  }

  glm::vec3 _basePoint;
};

std::array<size_t,3> Triangle::sortIndices(glm::vec3 refPoint) const {
  std::array<size_t,3> res;

  if (refPoint == vertices[0]->pos) {
    res[0] = 0;
    res[1] = 1;
    res[2] = 2;
  }

  else if (refPoint == vertices[1]->pos) {
    res[0] = 1;
    res[1] = 2;
    res[2] = 0;
  }

  else if (refPoint == vertices[2]->pos) {
    res[0] = 2;
    res[1] = 0;
    res[2] = 1;
  }

  else
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error in Triangle::sortIndices, no match for refPoint in triangle");

  return res;
}

const Triangle* Triangle::getTriangleContaining(
  glm::vec2 pos, const std::list<const Triangle*>& triangles, float* barCoord) {

  for (auto tri = triangles.begin(); tri != triangles.end(); tri++) {
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
      if (barCoord != nullptr) {
        barCoord[0] = s;
        barCoord[1] = t;
        barCoord[2] = 1-s-t;
      }

      return *tri;
    }
  }

  return nullptr;
}

void Vertex::sortTriangles() {
  if (!_sorted) {
    _sorted = true;
    _adjacentTriangles.sort(compTriClockwiseOrder(pos));
  }
}

const Triangle* Vertex::getNextTri(const Triangle* tri) const {
  std::list<const Triangle*>::const_iterator res = std::find(_adjacentTriangles.begin(), _adjacentTriangles.end(), tri);

  if (res == _adjacentTriangles.end()) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error in Vertex::getNextTri, current point does not belong to sent triangle");
    return nullptr;
  }

  else if (std::next(res) == _adjacentTriangles.end()) {
    std::array<size_t,3> srtCur = (*res)->sortIndices(pos);
    std::array<size_t,3> srtNxt = _adjacentTriangles.front()->sortIndices(pos);

    if ((*res)->vertices[srtCur[2]] != _adjacentTriangles.front()->vertices[srtNxt[1]])
      return nullptr;

    else
      return _adjacentTriangles.front();
  }

  else {
    std::array<size_t,3> srtCur = (*res)->sortIndices(pos);
    std::array<size_t,3> srtNxt = (*std::next(res))->sortIndices(pos);

    if ((*res)->vertices[srtCur[2]] != (*std::next(res))->vertices[srtNxt[1]])
      return nullptr;

    else
      return *std::next(res);
  }
}

std::pair<glm::vec3,glm::vec3> Vertex::getBorder() const {

  std::list<const Triangle*>::const_iterator t = _adjacentTriangles.begin();
  for (; std::next(t) != _adjacentTriangles.end(); t++) {
    std::array<size_t,3> srtCur = (*t)->sortIndices(pos);
    std::array<size_t,3> srtNxt = (*std::next(t))->sortIndices(pos);

    if ((*t)->vertices[srtCur[2]] != (*std::next(t))->vertices[srtNxt[1]]) {
      return std::pair<glm::vec3,glm::vec3>(
        (*t)->vertices[srtCur[2]]->pos,
        (*std::next(t))->vertices[srtNxt[1]]->pos
      );
    }
  }

  std::array<size_t,3> srtCur = (*t)->sortIndices(pos);
  std::array<size_t,3> srtNxt = _adjacentTriangles.front()->sortIndices(pos);

  if ((*t)->vertices[srtCur[2]] != _adjacentTriangles.front()->vertices[srtNxt[1]]) {
    return std::pair<glm::vec3,glm::vec3>(
      (*t)->vertices[srtCur[2]]->pos,
      _adjacentTriangles.front()->vertices[srtNxt[1]]->pos
    );
  }

  else
    return std::pair<glm::vec3,glm::vec3>();
}

void Vertex::addAdjacentTriangle(const Triangle* tri) {
  _adjacentTriangles.push_back(tri);
  _sorted = false;
}

TerrainGeometry::SubdivisionLevel::SubdivisionLevel(const GeneratedImage* relief) :
  _relief(relief) {
  std::vector<std::list<const Triangle*> > initializer(GRID_SUBDIV*GRID_SUBDIV);
  _trianglesInSubChunk.resize(NB_CHUNKS*NB_CHUNKS, initializer);
}

void TerrainGeometry::SubdivisionLevel::addTriangle(std::array<glm::vec3,3> p, Biome biome) {
  if (_relief != nullptr) {
    for (size_t i = 0; i < 3; i++) {
      p[i].z = PERLIN_HEIGHT_FACTOR * _relief->getValueNormalizedCoord(p[i].x / MAX_COORD, p[i].y / MAX_COORD);
    }
  }

  // Add the triangle to the list of all triangles
  Triangle newTriangle;
  newTriangle.biome = biome;
  glm::vec3 normal = glm::cross(p[1]-p[0],p[2]-p[0]);
  newTriangle.normal = normal /= glm::length(normal);

  // If the order of the vertices is wrong, fix it
  if (glm::dot(normal, glm::vec3(0,0,1)) < 0) {
    std::swap(p[1],p[2]);
    newTriangle.normal = - 1.f * newTriangle.normal;
  }

  // Add the vertices to the list of all vertices
  for (size_t i = 0; i < 3; i++) {
    if (_vertices.find(p[i]) == _vertices.end()) {
      Vertex vertex;
      vertex.pos = p[i];

      _vertices.insert(std::pair<glm::vec3, Vertex>(p[i], vertex));
    }

    newTriangle.vertices[i] = &_vertices.at(p[i]);
  }

  // Add the triangle if it is not stored yet
  std::pair<std::unordered_set<Triangle,triHashFunc>::iterator,bool> addedNewTri = _triangles.insert(newTriangle);

  if (addedNewTri.second) {

    for (size_t i = 0; i < 3; i++) {
      _vertices.at(p[i]).addAdjacentTriangle(&(*addedNewTri.first));
    }

    // Sort the triangle to place it in the adequate subchunks according to its bounding box

    glm::vec2 minAbsCoord, maxAbsCoord;
    minAbsCoord.x = std::min(p[0].x, std::min(p[1].x, p[2].x));
    minAbsCoord.y = std::min(p[0].y, std::min(p[1].y, p[2].y));
    maxAbsCoord.x = std::max(p[0].x, std::max(p[1].x, p[2].x)); if (maxAbsCoord.x == MAX_COORD) maxAbsCoord.x--;
    maxAbsCoord.y = std::max(p[0].y, std::max(p[1].y, p[2].y)); if (maxAbsCoord.y == MAX_COORD) maxAbsCoord.y--;

    std::array<glm::uvec2, 2> minChunkCoords = getSubChunkInfo(minAbsCoord);
    std::array<glm::uvec2, 2> maxChunkCoords = getSubChunkInfo(maxAbsCoord);

    for (size_t i = minChunkCoords[0].x; i < maxChunkCoords[0].x+1; i++) {
    for (size_t j = minChunkCoords[0].y; j < maxChunkCoords[0].y+1; j++) {
      for (size_t k = (i == minChunkCoords[0].x ? minChunkCoords[1].x : 0);
                  k < (i == maxChunkCoords[0].x ? maxChunkCoords[1].x + 1 : GRID_SUBDIV); k++) {
      for (size_t l = (j == minChunkCoords[0].y ? minChunkCoords[1].y : 0);
                  l < (j == maxChunkCoords[0].y ? maxChunkCoords[1].y + 1 : GRID_SUBDIV); l++) {


        _trianglesInSubChunk[i*NB_CHUNKS + j][k*GRID_SUBDIV + l].push_back(&(*addedNewTri.first));
      }
      }
    }
    }
  }
}

void TerrainGeometry::SubdivisionLevel::subdivideTriangles(std::list<const Triangle*>& triangles) {

  // Contains the modified vertices from the previous mesh (not the ones added on the edges)
  std::unordered_map<glm::vec3, glm::vec3, vertHashFunc> tmpProcessedVertices;

  for (std::list<const Triangle*>::iterator t = triangles.begin(); t != triangles.end(); t++) {

    // For every vertex that has not been processed, we process it and then save it
    for (size_t i = 0; i < 3; i++) {
      if (tmpProcessedVertices.find((*t)->vertices[i]->pos) == tmpProcessedVertices.end()) {
        (*t)->vertices[i]->sortTriangles();
        glm::vec3 newPos = (*t)->vertices[i]->pos;

        std::pair<glm::vec3,glm::vec3> border = (*t)->vertices[i]->getBorder();

        if (border.first != glm::vec3(0,0,0)) {
          newPos = 3.f/4.f * newPos + 1.f/8.f * border.first + 1.f/8.f * border.second;
        }

        else {
          float beta;
          int n = (*t)->vertices[i]->getAdjacentTriangles().size();

          if (n == 3)
          beta = 3.f/16.f;
          else
          beta = 3.f/(n*8.f);

          // The average is only done on the height z to preserve the summits
          newPos *= 1 - n*beta;

          // Average over the surrounding points
          for (auto neighbT  = (*t)->vertices[i]->getAdjacentTriangles().begin();
          neighbT != (*t)->vertices[i]->getAdjacentTriangles().end(); neighbT++) {

            std::array<size_t,3> srt = (*neighbT)->sortIndices((*t)->vertices[i]->pos);

            newPos += beta * (*neighbT)->vertices[srt[1]]->pos;
          }
        }

        tmpProcessedVertices.insert(std::pair<glm::vec3, glm::vec3>((*t)->vertices[i]->pos, newPos));
      }
    }

    std::array<glm::vec3,3> newMidPoints; // To construct the central triangle at the end
    for (size_t i = 0; i < 3; i++) {

      const Triangle* nextTri = (*t)->vertices[i]->getNextTri(*t);

      /*  i+1 p[i+2] i+2
       *   |  t      /|
       * p[i+1]   /   |
       *   |   p[i]   |  p = newMidPoints
       *   | /   nxtT |  nxtT = nextTri
       *   i --------
       */

      if (nextTri != nullptr && nextTri->biome == (*t)->biome) {
        std::array<size_t,3> srt = nextTri->sortIndices((*t)->vertices[i]->pos);

        newMidPoints[i] = 3.f/8.f * ((*t)->vertices[i]->pos + (*t)->vertices[(i+2)%3]->pos) +
                          1.f/8.f * ((*t)->vertices[(i+1)%3]->pos + nextTri->vertices[srt[2]]->pos);
      }

      else
        newMidPoints[i] = 1.f/2.f * ((*t)->vertices[i]->pos + (*t)->vertices[(i+2)%3]->pos);
    }

    for (size_t i = 0; i < 3; i++) {
      std::array<glm::vec3,3> newTrianglePositions = {
        tmpProcessedVertices[(*t)->vertices[i]->pos], newMidPoints[(i+1)%3], newMidPoints[i]};

      addTriangle(newTrianglePositions, (*t)->biome);
    }

    addTriangle(newMidPoints, (*t)->biome);
  }
}

void TerrainGeometry::SubdivisionLevel::computeNormals(std::list<Vertex*>& vertices) {
  for (auto p = vertices.begin(); p != vertices.end(); p++) {
    glm::vec3 normal(0.f,0.f,0.f);

    float totalWeight = 0;

    for (auto t = (*p)->getAdjacentTriangles().begin(); t != (*p)->getAdjacentTriangles().end(); t++) {
      float weight;

      std::array<size_t,3> srt = (*t)->sortIndices((*p)->pos);

      weight = glm::angle(glm::normalize((*t)->vertices[srt[1]]->pos-(*t)->vertices[srt[0]]->pos),
                          glm::normalize((*t)->vertices[srt[2]]->pos-(*t)->vertices[srt[0]]->pos));

      normal += weight * (*t)->normal;
      totalWeight += weight;
    }

    normal /= totalWeight;

    (*p)->normal = normal;
  }
}

void TerrainGeometry::SubdivisionLevel::computeNormals() {
  std::list<Vertex*> vertices;

  for (auto vert = _vertices.begin(); vert != _vertices.end(); vert++) {
    vertices.push_back(&(vert->second));
  }

  computeNormals(vertices);
}

bool TerrainGeometry::SubdivisionLevel::isOcean(size_t x, size_t y) const {
  for (size_t i = 0; i < _trianglesInSubChunk[x*NB_CHUNKS + y].size(); i++) {
    if (_trianglesInSubChunk[x*NB_CHUNKS + y][i].size() == 0)
      return true;
  }

  return false;
}

std::array<glm::uvec2, 2> TerrainGeometry::SubdivisionLevel::getSubChunkInfo(glm::vec2 pos) {
  std::array<glm::uvec2, 2> res;

  res[0].x = pos.x / CHUNK_SIZE;
  res[0].y = pos.y / CHUNK_SIZE;
  res[1].x = (pos.x - res[0].x * CHUNK_SIZE) / (CHUNK_SIZE / GRID_SUBDIV);
  res[1].y = (pos.y - res[0].y * CHUNK_SIZE) / (CHUNK_SIZE / GRID_SUBDIV);

  return res;
}

struct compTri {
  bool operator()(const Triangle* lhs, const Triangle* rhs) const {
  for (size_t i = 0; i < 3; i++) {
    if (lhs->vertices[i] < rhs->vertices[i])
      return true;
    else if (lhs->vertices[i] > rhs->vertices[i])
      return false;
  }

  return false;
  }
};

struct equalTri {
  bool operator()(const Triangle* lhs, const Triangle* rhs) {
  return ( lhs->vertices == rhs->vertices);
  }
};

std::list<Vertex*> TerrainGeometry::SubdivisionLevel::getVertices(const std::list<const Triangle*> triangles) {
  std::list<Vertex*> res;

  for (auto t = triangles.begin(); t != triangles.end(); t++) {
    for (size_t i = 0; i < 3; i++) {
      res.push_back((*t)->vertices[i]);
    }
  }

  res.unique();

  return res;
}

std::list<const Triangle*> TerrainGeometry::SubdivisionLevel::getTrianglesInChunk(size_t x, size_t y) const {
  std::list<const Triangle*> res;

  for (size_t i = 0; i < _trianglesInSubChunk[x*NB_CHUNKS + y].size(); i++) {
    res.insert(res.end(), _trianglesInSubChunk[x*NB_CHUNKS + y][i].begin(),
                          _trianglesInSubChunk[x*NB_CHUNKS + y][i].end());
  }

  res.sort(compTri());
  res.unique(equalTri());

  return res;
}

std::list<const Triangle*> TerrainGeometry::SubdivisionLevel::getTrianglesNearPos(glm::vec2 pos) const {
  std::array<glm::uvec2, 2> intCoord = getSubChunkInfo(pos);
  return _trianglesInSubChunk[intCoord[0].x*NB_CHUNKS  + intCoord[0].y]
                             [intCoord[1].x*GRID_SUBDIV + intCoord[1].y];
}

std::list<const Triangle*> TerrainGeometry::SubdivisionLevel::getTriangles() const {
  std::list<const Triangle*> triangles;
  for (auto t = _triangles.begin(); t != _triangles.end(); t++) {
    triangles.push_back(&(*t));
  }

  return triangles;
}

bool TerrainGeometry::SubdivisionLevel::isWater(glm::vec2 pos) const {
  const Triangle* t = Triangle::getTriangleContaining(pos, getTrianglesNearPos(pos));

  if (t == nullptr)
    return true;

  return t->biome == OCEAN || t->biome == WATER || t->biome == LAKE || t->biome == MARSH || t->biome == RIVER;
}

float TerrainGeometry::SubdivisionLevel::getHeight(glm::vec2 pos) const {
  float barCoord[3];
  const Triangle* t = Triangle::getTriangleContaining(pos, getTrianglesNearPos(pos), barCoord);

  if (t == nullptr)
    return 0;

  return barCoord[0]*t->vertices[0]->pos.z +
         barCoord[1]*t->vertices[1]->pos.z +
         barCoord[2]*t->vertices[2]->pos.z;
}

Biome TerrainGeometry::SubdivisionLevel::getBiome(glm::vec2 pos) const {
  const Triangle* triContaining = Triangle::getTriangleContaining(pos, getTrianglesNearPos(pos));

  return triContaining == nullptr ? OCEAN : triContaining->biome;
}

glm::vec3 TerrainGeometry::SubdivisionLevel::getNorm(glm::vec2 pos) const {
  float barCoord[3];
  const Triangle* t = Triangle::getTriangleContaining(pos, getTrianglesNearPos(pos), barCoord);

  if (t == nullptr)
    return glm::vec3(0,0,1);

  return barCoord[0]*t->vertices[0]->normal +
         barCoord[1]*t->vertices[1]->normal +
         barCoord[2]*t->vertices[2]->normal;
}

TerrainGeometry::TerrainGeometry() :
  _chunkSubdivLvl(NB_CHUNKS*NB_CHUNKS, 0),
  _currentGlobalSubdivLvl(0),
  _relief(std::vector<float>(1,0)) { // before giving the right relief generator, none is given

  // The first subdivision level should accept the geometry given as is
  _subdivisionLevels.push_back(std::unique_ptr<SubdivisionLevel>(new SubdivisionLevel(nullptr)));

  for (size_t i = 1; i < MAX_SUBDIV_LVL+1; i++) {
    _subdivisionLevels.push_back(std::unique_ptr<SubdivisionLevel>(new SubdivisionLevel(&_relief)));
  }
}

void TerrainGeometry::generateNewSubdivisionLevel() {
  if (_currentGlobalSubdivLvl < MAX_SUBDIV_LVL) {
    SubdivisionLevel* currentLvl = _subdivisionLevels[_currentGlobalSubdivLvl].get();
    SubdivisionLevel* nextLvl    = _subdivisionLevels[_currentGlobalSubdivLvl+1].get();

    std::list<const Triangle*> currentTriangles = currentLvl->getTriangles();

    nextLvl->goingToAddNPoints(currentTriangles.size() * 2);
    nextLvl->subdivideTriangles(currentTriangles);
    nextLvl->computeNormals();

    _currentGlobalSubdivLvl++;

    for (size_t i = 0; i < NB_CHUNKS*NB_CHUNKS; i++) {
      if (_chunkSubdivLvl[i] < _currentGlobalSubdivLvl)
        _chunkSubdivLvl[i] = _currentGlobalSubdivLvl;
    }
  }
}

void TerrainGeometry::subdivideChunk(int x, int y, size_t subdivLvl) {
  if ( x >= 0 && x < NB_CHUNKS && y >= 0 && y < NB_CHUNKS ) {
    if (_chunkSubdivLvl[x*NB_CHUNKS + y ] < subdivLvl) {
      subdivideChunk(x-1,y-1,subdivLvl-1);
      subdivideChunk(x-1,y  ,subdivLvl-1);
      subdivideChunk(x-1,y+1,subdivLvl-1);
      subdivideChunk(x  ,y-1,subdivLvl-1);
      subdivideChunk(x  ,y  ,subdivLvl-1);
      subdivideChunk(x  ,y+1,subdivLvl-1);
      subdivideChunk(x+1,y-1,subdivLvl-1);
      subdivideChunk(x+1,y  ,subdivLvl-1);
      subdivideChunk(x+1,y+1,subdivLvl-1);

      std::list<const Triangle*> toSubdivide = _subdivisionLevels[subdivLvl-1]->getTrianglesInChunk(x,y);
      _subdivisionLevels[subdivLvl]->subdivideTriangles(toSubdivide);

      std::list<Vertex*> vertices = SubdivisionLevel::getVertices(_subdivisionLevels[subdivLvl]->getTrianglesInChunk(x,y));
      _subdivisionLevels[subdivLvl]->computeNormals(vertices);

      _chunkSubdivLvl[x*NB_CHUNKS + y] = subdivLvl;
    }
  }
}

std::list<const Triangle*> TerrainGeometry::getTrianglesInChunk(size_t x, size_t y, size_t subdivLvl) {
  if (subdivLvl > MAX_SUBDIV_LVL)
    subdivLvl = MAX_SUBDIV_LVL;

  subdivideChunk(x,y,subdivLvl);

  return _subdivisionLevels[subdivLvl]->getTrianglesInChunk(x,y);
}

size_t TerrainGeometry::protectedSubdivLvl(glm::vec2 pos, size_t subdivLvl) const {
  std::array<glm::uvec2, 2> intCoord = SubdivisionLevel::getSubChunkInfo(pos);

  size_t currentSubdivLvl = _chunkSubdivLvl[intCoord[0].x*NB_CHUNKS  + intCoord[0].y];

  if (subdivLvl > currentSubdivLvl)
    subdivLvl = currentSubdivLvl;

  return subdivLvl;
}
