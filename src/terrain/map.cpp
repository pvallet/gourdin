#include "map.h"
#include "utils.h"
#include "vecUtils.h"
#include <omp.h>
#include <tinyxml.h>
#include <iostream>
#include <sstream>

#define MAP_MAX_COORD 600.

Map::Map() :
	_nbChunks(50),
	_maxCoord(_nbChunks * CHUNK_SIZE) {}

void Map::load(std::string path) {
	_minimap = new sf::Texture();

	std::ostringstream texPath;
  texPath << path << "map.png";

  sf::Image mapImg;

  if (!mapImg.loadFromFile(texPath.str())) {
    std::cerr << "Unable to open file: " << path << std::endl;
  }

  _minimap->loadFromImage(mapImg);
	_minimap->setSmooth(true);

  // Parse the XML file

  std::ostringstream xmlPath;
  xmlPath << path << "map.xml";

  TiXmlDocument doc(xmlPath.str());
  if(!doc.LoadFile()) {
    std::cerr << "Error while loading file: " << xmlPath.str() << std::endl;
    std::cerr << "Error #" << doc.ErrorId() << ": " << doc.ErrorDesc() << std::endl;
  }

  TiXmlHandle hDoc(&doc);
	TiXmlElement *elem, *elem2;
	TiXmlHandle hRoot(0);
	TiXmlHandle hSub(0);

	elem = hDoc.FirstChildElement().Element();
	if (!elem) return;

	int size;
	elem->QueryIntAttribute("size", &size);
	_centers.resize(size);

	hRoot = TiXmlHandle(elem);

	// Centers
	Center center;
	std::string str;
	int id;
	elem = hRoot.FirstChild("centers").FirstChild().Element();
	for (; elem; elem = elem->NextSiblingElement()) {
		elem->QueryIntAttribute("id", &center.id);
		elem->QueryDoubleAttribute("x", &center.x);
		elem->QueryDoubleAttribute("y", &center.y);

		elem->QueryStringAttribute("water", &str);
		center.water = boolAttrib(str);
		elem->QueryStringAttribute("ocean", &str);
		center.ocean = boolAttrib(str);
		elem->QueryStringAttribute("coast", &str);
		center.coast = boolAttrib(str);
		elem->QueryStringAttribute("border", &str);
		center.border = boolAttrib(str);

		elem->QueryStringAttribute("biome", &str);
		center.biome = biomeAttrib(str);

		elem->QueryDoubleAttribute("elevation", &center.elevation);
		elem->QueryDoubleAttribute("moisture", &center.moisture);


		hSub = TiXmlHandle(elem);
		elem2 = hSub.FirstChildElement().Element();

		center.centerIDs.clear();
		center.edgeIDs.clear();
		center.cornerIDs.clear();

		for (; elem2 && elem2->Value() == std::string("center"); elem2 = elem2->NextSiblingElement()) {
			elem2->QueryIntAttribute("id", &id);
			center.centerIDs.push_back(id);
		}

		for (; elem2 && elem2->Value() == std::string("edge"); elem2 = elem2->NextSiblingElement()) {
			elem2->QueryIntAttribute("id", &id);
			center.edgeIDs.push_back(id);
		}

		for (; elem2 && elem2->Value() == std::string("corner"); elem2 = elem2->NextSiblingElement()) {
			elem2->QueryIntAttribute("id", &id);
			center.cornerIDs.push_back(id);
		}

		center.centers.resize(center.centerIDs.size());
		center.edges.  resize(center.edgeIDs.size());
		center.corners.resize(center.cornerIDs.size());

		_centers[center.id] = new Center(center);
	}

	// Edges
	Edge edge;
	elem = hRoot.FirstChild("edges").FirstChild().Element();
	for(; elem; elem = elem->NextSiblingElement()) {
		elem->QueryIntAttribute("id", &edge.id);
		elem->QueryIntAttribute("river", &edge.river);
		elem->QueryIntAttribute("center0", &edge.center0ID);
		elem->QueryIntAttribute("center1", &edge.center1ID);

		// If the edge touches the edge of the map, there are no ends
		if (elem->QueryDoubleAttribute("x", &edge.x) == TIXML_NO_ATTRIBUTE) {
			edge.mapEdge = true;
			edge.x = 0.;
			edge.y = 0.;
			edge.corner0 = 0;
			edge.corner1 = 0;
		}

		else {
			edge.mapEdge = false;
			elem->QueryDoubleAttribute("y", &edge.y);
			elem->QueryIntAttribute("corner0", &edge.corner0ID);
			elem->QueryIntAttribute("corner1", &edge.corner1ID);
		}

		if (edge.id >= (int) _edges.size())
			_edges.resize(edge.id + 1);
		_edges[edge.id] = new Edge(edge);
	}

	// Corners
	Corner corner;
	elem = hRoot.FirstChild("corners").FirstChild().Element();
	for(; elem; elem = elem->NextSiblingElement()) {
		elem->QueryIntAttribute("id", &corner.id);
		elem->QueryDoubleAttribute("x", &corner.x);
		elem->QueryDoubleAttribute("y", &corner.y);

		elem->QueryStringAttribute("water", &str);
		corner.water = boolAttrib(str);
		elem->QueryStringAttribute("ocean", &str);
		corner.ocean = boolAttrib(str);
		elem->QueryStringAttribute("coast", &str);
		corner.coast = boolAttrib(str);
		elem->QueryStringAttribute("border", &str);
		corner.border = boolAttrib(str);

		elem->QueryDoubleAttribute("elevation", &corner.elevation);
		elem->QueryDoubleAttribute("moisture", &corner.moisture);

		elem->QueryIntAttribute("river", &corner.river);
		elem->QueryIntAttribute("downslope", &corner.downslope);

		hSub = TiXmlHandle(elem);
		elem2 = hSub.FirstChildElement().Element();

		corner.centerIDs.clear();
		corner.edgeIDs.clear();
		corner.cornerIDs.clear();

		for ( ; elem2 && elem2->Value() == std::string("center"); elem2 = elem2->NextSiblingElement()) {
			elem2->QueryIntAttribute("id", &id);
			corner.centerIDs.push_back(id);
		}

		for ( ; elem2 && elem2->Value() == std::string("edge"); elem2 = elem2->NextSiblingElement()) {
			elem2->QueryIntAttribute("id", &id);
			corner.edgeIDs.push_back(id);
		}

		for ( ; elem2 && elem2->Value() == std::string("corner"); elem2 = elem2->NextSiblingElement()) {
			elem2->QueryIntAttribute("id", &id);
			corner.cornerIDs.push_back(id);
		}

		corner.centers.resize(corner.centerIDs.size());
		corner.edges.  resize(corner.edgeIDs.size());
		corner.corners.resize(corner.cornerIDs.size());

		if (corner.id >= (int) _corners.size())
			_corners.resize(corner.id + 1);
		_corners[corner.id] = new Corner(corner);
	}

	// Convert to game coordinates
	#pragma omp parallel for
	for (unsigned int i = 0 ; i < _centers.size() ; i++) {
		_centers[i]->x *= _maxCoord / MAP_MAX_COORD;
		_centers[i]->y *= _maxCoord / MAP_MAX_COORD;
	}

	for (unsigned int i = 0 ; i < _edges.size() ; i++) {
		if (!_edges[i]->mapEdge) {
			_edges[i]->x *= _maxCoord / MAP_MAX_COORD;
			_edges[i]->y *= _maxCoord / MAP_MAX_COORD;
		}
	}

	for (unsigned int i = 0 ; i < _corners.size() ; i++) {
		_corners[i]->x *= _maxCoord / MAP_MAX_COORD;
		_corners[i]->y *= _maxCoord / MAP_MAX_COORD;
	}

	// Set the pointers thanks to the indices
	#pragma omp parallel for
	for (unsigned int i = 0 ; i < _centers.size() ; i++) {
		for (unsigned int j = 0 ; j < _centers[i]->centerIDs.size() ; j++) {
			_centers[i]->centers[j] = _centers[_centers[i]->centerIDs[j]];
		}

		for (unsigned int j = 0 ; j < _centers[i]->edgeIDs.size() ; j++) {
			_centers[i]->edges[j] = _edges[_centers[i]->edgeIDs[j]];
		}

		for (unsigned int j = 0 ; j < _centers[i]->cornerIDs.size() ; j++) {
			_centers[i]->corners[j] = _corners[_centers[i]->cornerIDs[j]];
		}
	}

	#pragma omp parallel for
	for (unsigned int i = 0 ; i < _edges.size() ; i++) {
		if (!_edges[i]->mapEdge) {
			_edges[i]->center0 = _centers[_edges[i]->center0ID];
			_edges[i]->center1 = _centers[_edges[i]->center1ID];
			_edges[i]->corner0 = _corners[_edges[i]->corner0ID];
			_edges[i]->corner1 = _corners[_edges[i]->corner1ID];
		}
	}

	#pragma omp parallel for
	for (unsigned int i = 0 ; i < _corners.size() ; i++) {
		for (unsigned int j = 0 ; j < _corners[i]->centerIDs.size() ; j++) {
			_corners[i]->centers[j] = _centers[_corners[i]->centerIDs[j]];
		}

		for (unsigned int j = 0 ; j < _corners[i]->edgeIDs.size() ; j++) {
			_corners[i]->edges[j] = _edges[_corners[i]->edgeIDs[j]];
		}

		for (unsigned int j = 0 ; j < _corners[i]->cornerIDs.size() ; j++) {
			_corners[i]->corners[j] = _corners[_corners[i]->cornerIDs[j]];
		}
	}


	_data = new double[_centers.size()*2];

	for (unsigned int i = 0 ; i < _centers.size() ; i++) {
		_data[2*i]   = _centers[i]->x;
		_data[2*i+1] = _centers[i]->y;
	}

	_dataset = flann::Matrix<double>(_data, _centers.size(), 2);

	_kdIndex = new flann::Index<flann::L2<double> >(_dataset, flann::KDTreeIndexParams(4));

	_kdIndex->buildIndex();
}

bool Map::boolAttrib(std::string str) const {
	if (str == std::string("true"))
		return true;
	else
		return false;
}

Biome Map::biomeAttrib(std::string str) const {
	if (str == std::string("OCEAN"))
		return OCEAN;
	else if (str == std::string("WATER"))
		return WATER;
	else if (str == std::string("LAKE"))
		return LAKE;
	else if (str == std::string("ICE"))
		return ICE;
	else if (str == std::string("MARSH"))
		return MARSH;
	else if (str == std::string("BEACH"))
		return BEACH;
	else if (str == std::string("RIVER"))
		return RIVER;
	else if (str == std::string("SNOW"))
		return SNOW;
	else if (str == std::string("TUNDRA"))
		return TUNDRA;
	else if (str == std::string("BARE"))
		return BARE;
	else if (str == std::string("SCORCHED"))
		return SCORCHED;
	else if (str == std::string("TAIGA"))
		return TAIGA;
	else if (str == std::string("SHRUBLAND"))
		return SHRUBLAND;
	else if (str == std::string("TEMPERATE_DESERT"))
		return TEMPERATE_DESERT;
	else if (str == std::string("TEMPERATE_RAIN_FOREST"))
		return TEMPERATE_RAIN_FOREST;
	else if (str == std::string("TEMPERATE_DECIDUOUS_FOREST"))
		return TEMPERATE_DECIDUOUS_FOREST;
	else if (str == std::string("GRASSLAND"))
		return GRASSLAND;
	else if (str == std::string("TROPICAL_RAIN_FOREST"))
		return TROPICAL_RAIN_FOREST;
	else if (str == std::string("TROPICAL_SEASONAL_FOREST"))
		return TROPICAL_SEASONAL_FOREST;
	else if (str == std::string("SUBTROPICAL_DESERT"))
		return SUBTROPICAL_DESERT;
	else
		return GRASSLAND;
}

Map::~Map() {
	for (unsigned int i = 0 ; i < _centers.size() ; i++) {
		// delete _centers[i];
	}

	for (unsigned int i = 0 ; i < _edges.size() ; i++) {
		// delete _edges[i];
	}

	for (unsigned int i = 0 ; i < _corners.size() ; i++) {
		// delete _corners[i];
	}

	// delete[] _data;
	// delete[] _dataset.ptr();
	// delete[] _kdIndex;
}

Center* Map::getClosestCenter(sf::Vector2<double> pos) const {
	double queryData[2];
	queryData[0] = pos.x;
	queryData[1] = pos.y;

	flann::Matrix<double> query(queryData, 1, 2);

  std::vector<std::vector<int> > indices;
  std::vector<std::vector<double> > dists;

  _kdIndex->knnSearch(query, indices, dists, 1, flann::SearchParams(_centers.size()));

	return _centers[indices[0][0]];
}

std::vector<Center*> Map::getCentersInChunk(sf::Vector2i chunkPos) const {
	double queryData[2];
	queryData[0] = chunkPos.x * CHUNK_SIZE + CHUNK_SIZE / 2;
	queryData[1] = chunkPos.y * CHUNK_SIZE + CHUNK_SIZE / 2;

	flann::Matrix<double> query(queryData, 1, 2);

  std::vector<std::vector<int> > indices;
  std::vector<std::vector<double> > dists;

  // Flann weirdly expects the square of the radius
  _kdIndex->radiusSearch(query, indices, dists, CHUNK_SIZE*CHUNK_SIZE/2, flann::SearchParams(_centers.size()));

  std::vector<Center*> res;
  for (unsigned int i = 0 ; i < indices[0].size() ; i++) {

  	if (_centers[indices[0][i]]->x >= chunkPos.x * CHUNK_SIZE &&
  		_centers[indices[0][i]]->x <= (chunkPos.x+1) * CHUNK_SIZE &&
  		_centers[indices[0][i]]->y >= chunkPos.y * CHUNK_SIZE &&
  		_centers[indices[0][i]]->y <= (chunkPos.y+1) * CHUNK_SIZE) {

  		res.push_back(_centers[indices[0][i]]);
  	}
  }

	return res;
}
