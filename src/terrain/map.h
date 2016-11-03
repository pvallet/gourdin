#pragma once

#include <SFML/System.hpp>
#include <flann/flann.hpp>

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "utils.h"
#include "vecUtils.h"

class TiXmlHandle;

struct Center;
struct Edge;
struct Corner;

struct Center {
	int id;
	float x;
	float y;
	bool water;
	bool ocean;
	bool coast;
	bool border;
	Biome biome;
	float elevation;
	float moisture;

	std::vector<int> centerIDs; // Only for initialisation
	std::vector<int> edgeIDs;
	std::vector<int> cornerIDs;

	std::vector<Center*> centers;
	std::vector<Edge*> edges;
	std::vector<Corner*> corners;
};

class Edge {
public:
	Edge() {}

	int id;
	bool mapEdge;
	float x; // Midpoint coordinates
	float y;
	int river;

	float beginX;
	float beginY;
	float endX;
	float endY;

	int center0ID; // Only for initialisation
	int center1ID;
	int corner0ID;
	int corner1ID;

	Center* center0;
	Center* center1;
	Corner* corner0;
	Corner* corner1;

	sf::Vector2f normalToCenter0; // set in computeEdgeBoundingBoxes

	// Minimal distance to any point of the edge. Iso distances are half disk U rectangle U half disk
	float getDistanceToEdge(sf::Vector2f pos);
	bool isOnSameSideAsCenter0(sf::Vector2f pos) const;

};

struct Corner {
	int id;
	float x;
	float y;
	bool water;
	bool ocean;
	bool coast;
	bool border;
	float elevation;
	float moisture;
	int river;
	int downslope; // Index of the lowest adjacent corner

	std::vector<int> centerIDs; // Only for initialisation
	std::vector<int> edgeIDs;
	std::vector<int> cornerIDs;

	std::vector<Center*> centers;
	std::vector<Edge*> edges;
	std::vector<Corner*> corners;
};

struct CentersInChunk {
	std::vector<float> data;
	flann::Matrix<float> dataset; // For knn searches
	std::unique_ptr<flann::Index<flann::L2<float> > > kdIndex;

	std::vector<Center*> centers;
	bool isOcean;
};

class Map {
public:
	Map() {}

	void load(std::string path);

	Center* getClosestCenter(sf::Vector2f pos) const;
	std::vector<Center*>               getCentersInChunk(size_t x, size_t y) const;
	inline const std::set<Edge*>&      getEdgesInChunk  (size_t x, size_t y) const {
		return _edgesInChunks[x*NB_CHUNKS + y];
	}
	inline const std::vector<Corner*>& getCornersInChunk(size_t x, size_t y) const {
		return _cornersInChunks[x*NB_CHUNKS + y];
	}

	inline bool isOcean(size_t x, size_t y) const {
		return _centersInChunks[x*NB_CHUNKS + y].isOcean;
	}

private:
	void loadCenters(const TiXmlHandle& hRoot);
	void loadEdges(const TiXmlHandle& hRoot);
	void loadCorners(const TiXmlHandle& hRoot);
	void setPointersInDataStructures();
	void sortCenters(float tolerance);
	void computeEdgeBoundingBoxes();
	void sortEdges();
	void sortCorners();
	bool boolAttrib(std::string str) const;
	Biome biomeAttrib(std::string str) const;

	std::vector<std::unique_ptr<Center> > _centers;
	std::vector<std::unique_ptr<Edge> >   _edges;
	std::vector<std::unique_ptr<Corner> > _corners;

	// Contains the centers that are near to a given chunk. The chunks are sorted
	// as chunk.x * NB_CHUNKS + chunk.y with tolerance computed by computeCenterToleranceToChunk
	std::vector<CentersInChunk> _centersInChunks;
	// Contains the edges that intersect the chunk with tolerance TERRAIN_TEX_TRANSITION_SIZE
	std::vector<std::set<Edge*> > _edgesInChunks;
	std::vector<std::vector<Corner*> > _cornersInChunks;
};
