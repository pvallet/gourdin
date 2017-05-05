#pragma once

#include <SFML/System.hpp>

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "terrainGeometry.h"

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

class Map {
public:
	Map() {}

	void load(std::string path);
	void feedGeometryData(TerrainGeometry& terrainGeometry) const;

private:
	bool boolAttrib(std::string str) const;
	Biome biomeAttrib(std::string str) const;
	void loadCenters(const TiXmlHandle& hRoot);
	void loadEdges(const TiXmlHandle& hRoot);
	void loadCorners(const TiXmlHandle& hRoot);
	void setPointersInDataStructures();

	std::vector<std::unique_ptr<Center> > _centers;
	std::vector<std::unique_ptr<Edge> >   _edges;
	std::vector<std::unique_ptr<Corner> > _corners;
};
