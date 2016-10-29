#pragma once

#include <flann/flann.hpp>

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "utils.h"

class TiXmlHandle;

struct Center;
struct Edge;
struct Corner;

struct Center {
	int id;
	double x;
	double y;
	bool water;
	bool ocean;
	bool coast;
	bool border;
	Biome biome;
	double elevation;
	double moisture;

	std::vector<int> centerIDs; // Only for initialisation
	std::vector<int> edgeIDs;
	std::vector<int> cornerIDs;

	std::vector<Center*> centers;
	std::vector<Edge*> edges;
	std::vector<Corner*> corners;
};

struct Edge {
	int id;
	bool mapEdge;
	double x; // Midpoint coordinates
	double y;
	int river;

	int center0ID; // Only for initialisation
	int center1ID;
	int corner0ID;
	int corner1ID;

	Center* center0;
	Center* center1;
	Corner* corner0;
	Corner* corner1;
};

struct Corner {
	int id;
	double x;
	double y;
	bool water;
	bool ocean;
	bool coast;
	bool border;
	double elevation;
	double moisture;
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
	std::vector<double> data;
	flann::Matrix<double> dataset; // For knn searches
	std::unique_ptr<flann::Index<flann::L2<double> > > kdIndex;

	std::vector<Center*> centers;
};

class Map {
public:
	Map() {}

	void load(std::string path);

	Center* getClosestCenter(sf::Vector2<double> pos) const;
	std::vector<Center*> getCentersInChunk(size_t x, size_t y) const;

private:
	void loadCenters(const TiXmlHandle& hRoot);
	void loadEdges(const TiXmlHandle& hRoot);
	void loadCorners(const TiXmlHandle& hRoot);
	void setPointersInDataStructures();
	void sortCenters(float tolerance);
	bool boolAttrib(std::string str) const;
	Biome biomeAttrib(std::string str) const;

	std::vector<std::unique_ptr<Center> > _centers;
	std::vector<std::unique_ptr<Edge> >   _edges;
	std::vector<std::unique_ptr<Corner> > _corners;

	// Contains the centers that are near to a given chunk. The chunks are sorted
	// as chunk.x * NB_CHUNKS + chunk.y
	std::vector<CentersInChunk> _centersInChunks;
};
