#pragma once

#include <memory>
#include <string>
#include <vector>

#include "terrainGeometry.h"

#include "utils.h"

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

struct Edge {
	int id = 0;
	bool mapEdge = false;
	float x = 0.f; // Midpoint coordinates
	float y = 0.f;
	int river = 0;

	float beginX = 0.f;
	float beginY = 0.f;
	float endX = 0.f;
	float endY = 0.f;

	int center0ID = 0; // Only for initialisation
	int center1ID = 0;
	int corner0ID = 0;
	int corner1ID = 0;

	Center* center0 = nullptr;
	Center* center1 = nullptr;
	Corner* corner0 = nullptr;
	Corner* corner1 = nullptr;

	glm::vec2 normalToCenter0; // set in computeEdgeBoundingBoxes

	// Minimal distance to any point of the edge. Iso distances are half disk U rectangle U half disk
	float getDistanceToEdge(glm::vec2 pos);
	bool isOnSameSideAsCenter0(glm::vec2 pos) const;

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
