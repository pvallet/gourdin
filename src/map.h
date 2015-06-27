#pragma once

#include <SFML/Graphics.hpp>
#include <flann/flann.hpp>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include "camera.h"
#include "utils.h"

struct Center;
struct Edge;
struct Corner;

typedef struct Center Center;
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

typedef struct Edge Edge;
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

typedef struct Corner Corner;
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

class Map {
public:
	Map(std::string path);
	~Map();

	inline sf::Texture* getMinimap() const {return minimap;}
	inline std::vector<Center*> getCenters() const {return centers;}
	inline std::vector<Edge*> 	getEdges()   const {return edges;}
	inline std::vector<Corner*> getCorners() const {return corners;}
	inline int getNbChunks() const {return nbChunks;}
	inline double getMaxCoord() const {return maxCoord;}

	Center* getClosestCenter(sf::Vector2<double> pos) const;

private:
	bool boolAttrib(std::string str) const;
	Biome biomeAttrib(std::string str) const;

	sf::Texture* minimap;
	std::vector<Center*> centers;
	std::vector<Edge*>   edges;
	std::vector<Corner*> corners;

	int nbChunks; // Number of chunks on a row
	double maxCoord;

	double* data;
	flann::Matrix<double> dataset; // For knn searches
	flann::Index<flann::L2<double> >* kdIndex;
};

