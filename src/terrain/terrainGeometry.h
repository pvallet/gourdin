#pragma once

#include <glm/gtx/hash.hpp>

#include <array>
#include <list>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "generatedImage.h"
#include "utils.h"

#define MAX_SUBDIV_LVL 4

struct Vertex;

class Triangle {
public:
  std::array<Vertex*,3> vertices;
  Biome biome;
  glm::vec3 normal;

  // indice 0 corresponds to the point refPoint in tri, and the other indices are
  // the other vertices in clockwise order
  std::array<size_t,3> sortIndices(glm::vec3 refPoint) const;

  // Returns the triangle containing the given pos, and stores the barycentric
  // coordinates in barCoord: barCoord[i] corresponds to vertices[]
  // barCoord is expected to be a float array of size 3
  static const Triangle* getTriangleContaining(glm::vec2 pos,
    const std::list<const Triangle*>& triangles, float* barCoord = nullptr);
};

class Vertex {
public:
  Vertex(): _sorted(false) {}

  glm::vec3 pos;
  glm::vec3 normal;

  // If the triangles are not sorted yet, sorts them in clockwise order
  void sortTriangles();
  const Triangle* getNextTri(const Triangle* tri) const;
  std::pair<glm::vec3,glm::vec3> getBorder() const;

  void addAdjacentTriangle(const Triangle* tri);
  inline const std::list<const Triangle*>& getAdjacentTriangles() const {return _adjacentTriangles;}

private:
  bool _sorted;
  std::list<const Triangle*> _adjacentTriangles;
};

struct triHashFunc{
  size_t operator()(const Triangle& t) const {
  size_t h1 = std::hash<glm::vec3>()(t.vertices[0]->pos);
  size_t h2 = std::hash<glm::vec3>()(t.vertices[1]->pos);
  size_t h3 = std::hash<glm::vec3>()(t.vertices[2]->pos);
  return (h1 ^ (h2 << 1)) ^ h3;
  }
};

inline bool operator == (const Triangle& lhs, const Triangle& rhs) {
  return lhs.vertices == rhs.vertices;
}

// Handles the different levels of subdivision of the geometry of the terrain
class TerrainGeometry {
public:
  // Class to handle a single subdivision level
  class SubdivisionLevel {
  public:
    SubdivisionLevel (const GeneratedImage* relief);

    // Init methods
    inline void goingToAddNPoints(size_t n) {_vertices.reserve(n);}
    // Insert the triangle to the structure and modify its points to take into account the relief generator
    // If the relief generator is nullptr, the points are not modified
    void addTriangle(std::array<glm::vec3,3> p, Biome biome);
    void subdivideTriangles(std::list<const Triangle*>& triangles);
    void computeNormals    (std::list<Vertex*>&   vertices);
    // Compute normals for every triangle
    void computeNormals();

    bool isOcean(size_t x, size_t y) const;

    static std::list<Vertex*> getVertices(const std::list<const Triangle*> triangles);

    std::list<const Triangle*> getTrianglesInChunk(size_t x, size_t y) const;
    std::list<const Triangle*> getTrianglesNearPos  (glm::vec2 pos) const;

    std::list<const Triangle*> getTriangles() const;

    bool isWater(glm::vec2 pos) const;
    float getHeight(glm::vec2 pos) const;
    Biome getBiome (glm::vec2 pos) const;
    glm::vec3 getNorm(glm::vec2 pos) const;

    // Returns the coordinates of the subchunk containing the position pos
    static std::array<glm::uvec2, 2> getSubChunkInfo(glm::vec2 pos);

  private:

    std::unordered_map<glm::vec3, Vertex> _vertices;
    std::unordered_set<Triangle, triHashFunc> _triangles;

    // The triangles are sorted on a two level grid:
    // - The first level corresponds to the chunk containing the triangle (x_chunk*NB_CHUNKS + y_chunk)
    // - The second level corresponds to the subchunk (x_subchunk * GRID_SUBDIV + y_subchunk) (GRID_SUBDIV in cpp)
    // Each triangle can belong to several subchunks
    std::vector<std::vector<std::list<const Triangle*> > > _trianglesInSubChunk;

    const GeneratedImage* _relief;
  };

  TerrainGeometry ();
  inline void setReliefGenerator(GeneratedImage reliefGenerator) {_relief = reliefGenerator;}

  void generateNewSubdivisionLevel();

  inline const GeneratedImage& getReliefGenerator() const {return _relief;}

  // For initialization
  inline SubdivisionLevel* getFirstSubdivLevel() {return _subdivisionLevels[0].get();}
  inline const SubdivisionLevel* getFirstSubdivLevel() const {return _subdivisionLevels[0].get();}
  inline size_t getCurrentGlobalSubdivLvl() const {return _currentGlobalSubdivLvl;}
  std::list<const Triangle*> getTrianglesInChunk(size_t x, size_t y, size_t subdivLvl);

  inline bool isOcean(size_t x, size_t y) const {return _subdivisionLevels[0]->isOcean(x,y);}

  inline bool isWater(glm::vec2 pos, size_t subdivLvl) const  {
    return _subdivisionLevels[protectedSubdivLvl(pos, subdivLvl)]->isWater(pos);}

  inline float getHeight(glm::vec2 pos, size_t subdivLvl) const {
    return _subdivisionLevels[protectedSubdivLvl(pos, subdivLvl)]->getHeight(pos);}

  inline Biome getBiome(glm::vec2 pos, size_t subdivLvl) const {
    return _subdivisionLevels[protectedSubdivLvl(pos, subdivLvl)]->getBiome(pos);}

  inline glm::vec3 getNorm(glm::vec2 pos, size_t subdivLvl) const {
    return _subdivisionLevels[protectedSubdivLvl(pos, subdivLvl)]->getNorm(pos);}

private:
  void subdivideChunk(int x, int y, size_t subdivLvl);
  size_t protectedSubdivLvl(glm::vec2 pos, size_t subdivLvl) const;

  std::vector<std::unique_ptr<SubdivisionLevel> > _subdivisionLevels;

  // Deepest available subdivision level for every chunk (x_chunk*NB_CHUNKS + y_chunk)
  std::vector<size_t> _chunkSubdivLvl;

  size_t _currentGlobalSubdivLvl;

  GeneratedImage _relief;
};
