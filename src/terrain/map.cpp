#include "map.h"

#include "tinyxml.h"

#include <sstream>

// The coordinates system in the XML file ends at MAP_MAX_COORD
#define MAP_MAX_COORD 600.f

bool Map::boolAttrib(std::string str) const {
	if (str == std::string("true"))
		return true;
	else
		return false;
}

Biome Map::biomeAttrib(std::string str) const {
	if (str == std::string("OCEAN"))
		return Biome::OCEAN;
	else if (str == std::string("WATER"))
		return Biome::WATER;
	else if (str == std::string("LAKE"))
		return Biome::LAKE;
	else if (str == std::string("ICE"))
		return Biome::ICE;
	else if (str == std::string("MARSH"))
		return Biome::MARSH;
	else if (str == std::string("BEACH"))
		return Biome::BEACH;
	else if (str == std::string("RIVER"))
		return Biome::RIVER;
	else if (str == std::string("SNOW"))
		return Biome::SNOW;
	else if (str == std::string("TUNDRA"))
		return Biome::TUNDRA;
	else if (str == std::string("BARE"))
		return Biome::BARE;
	else if (str == std::string("SCORCHED"))
		return Biome::SCORCHED;
	else if (str == std::string("TAIGA"))
		return Biome::TAIGA;
	else if (str == std::string("SHRUBLAND"))
		return Biome::SHRUBLAND;
	else if (str == std::string("TEMPERATE_DESERT"))
		return Biome::TEMPERATE_DESERT;
	else if (str == std::string("TEMPERATE_RAIN_FOREST"))
		return Biome::TEMPERATE_RAIN_FOREST;
	else if (str == std::string("TEMPERATE_DECIDUOUS_FOREST"))
		return Biome::TEMPERATE_DECIDUOUS_FOREST;
	else if (str == std::string("GRASSLAND"))
		return Biome::GRASSLAND;
	else if (str == std::string("TROPICAL_RAIN_FOREST"))
		return Biome::TROPICAL_RAIN_FOREST;
	else if (str == std::string("TROPICAL_SEASONAL_FOREST"))
		return Biome::TROPICAL_SEASONAL_FOREST;
	else if (str == std::string("SUBTROPICAL_DESERT"))
		return Biome::SUBTROPICAL_DESERT;
	else
		return Biome::GRASSLAND;
}

void Map::loadCenters(const TiXmlHandle& hRoot) {
	TiXmlElement *elem, *elem2;
	TiXmlHandle hSub(0);

	Center center;
	std::string str;
	int id;
	elem = hRoot.FirstChild("centers").FirstChild().Element();
	for (; elem; elem = elem->NextSiblingElement()) {
		elem->QueryIntAttribute("id", &center.id);
		elem->QueryFloatAttribute("x", &center.x);
		elem->QueryFloatAttribute("y", &center.y);

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

		elem->QueryFloatAttribute("elevation", &center.elevation);
		elem->QueryFloatAttribute("moisture", &center.moisture);


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
		center.edges.  resize(center.edgeIDs.  size());
		center.corners.resize(center.cornerIDs.size());

		_centers[center.id] = std::unique_ptr<Center>(new Center(center));
	}
}

void Map::loadEdges(const TiXmlHandle& hRoot) {
	TiXmlElement *elem;
	Edge edge;
	elem = hRoot.FirstChild("edges").FirstChild().Element();
	for(; elem; elem = elem->NextSiblingElement()) {
		elem->QueryIntAttribute("id", &edge.id);
		elem->QueryIntAttribute("river", &edge.river);
		elem->QueryIntAttribute("center0", &edge.center0ID);
		elem->QueryIntAttribute("center1", &edge.center1ID);

		// If the edge touches the edge of the map, there are no ends
		if (elem->QueryFloatAttribute("x", &edge.x) == TIXML_NO_ATTRIBUTE) {
			edge.mapEdge = true;
			edge.x = 0.f;
			edge.y = 0.f;
		}

		else {
			edge.mapEdge = false;
			elem->QueryFloatAttribute("y", &edge.y);
			elem->QueryIntAttribute("corner0", &edge.corner0ID);
			elem->QueryIntAttribute("corner1", &edge.corner1ID);
		}

		if (edge.id >= (int) _edges.size())
			_edges.resize(edge.id + 1);
		_edges[edge.id] = std::unique_ptr<Edge>(new Edge(edge));
	}
}

void Map::loadCorners(const TiXmlHandle& hRoot) {
	TiXmlElement *elem, *elem2;
	TiXmlHandle hSub(0);

	Corner corner;
	std::string str;

	int id;

	elem = hRoot.FirstChild("corners").FirstChild().Element();
	for(; elem; elem = elem->NextSiblingElement()) {
		elem->QueryIntAttribute("id", &corner.id);
		elem->QueryFloatAttribute("x", &corner.x);
		elem->QueryFloatAttribute("y", &corner.y);

		elem->QueryStringAttribute("water", &str);
		corner.water = boolAttrib(str);
		elem->QueryStringAttribute("ocean", &str);
		corner.ocean = boolAttrib(str);
		elem->QueryStringAttribute("coast", &str);
		corner.coast = boolAttrib(str);
		elem->QueryStringAttribute("border", &str);
		corner.border = boolAttrib(str);

		elem->QueryFloatAttribute("elevation", &corner.elevation);
		elem->QueryFloatAttribute("moisture", &corner.moisture);

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
		_corners[corner.id] = std::unique_ptr<Corner>(new Corner(corner));
	}
}

void Map::setPointersInDataStructures() {
	#pragma omp parallel for
	for (int i = 0 ; i < _centers.size() ; i++) {
		for (int j = 0 ; j < _centers[i]->centerIDs.size() ; j++) {
			_centers[i]->centers[j] = _centers[_centers[i]->centerIDs[j]].get();
		}

		for (int j = 0 ; j < _centers[i]->edgeIDs.size() ; j++) {
			_centers[i]->edges[j] = _edges[_centers[i]->edgeIDs[j]].get();
		}

		for (int j = 0 ; j < _centers[i]->cornerIDs.size() ; j++) {
			_centers[i]->corners[j] = _corners[_centers[i]->cornerIDs[j]].get();
		}
	}

	#pragma omp parallel for
	for (int i = 0 ; i < _edges.size() ; i++) {
		if (!_edges[i]->mapEdge) {
			_edges[i]->center0 = _centers[_edges[i]->center0ID].get();
			_edges[i]->center1 = _centers[_edges[i]->center1ID].get();
			_edges[i]->corner0 = _corners[_edges[i]->corner0ID].get();
			_edges[i]->corner1 = _corners[_edges[i]->corner1ID].get();
		}
	}

	#pragma omp parallel for
	for (int i = 0 ; i < _corners.size() ; i++) {
		for (int j = 0 ; j < _corners[i]->centerIDs.size() ; j++) {
			_corners[i]->centers[j] = _centers[_corners[i]->centerIDs[j]].get();
		}

		for (int j = 0 ; j < _corners[i]->edgeIDs.size() ; j++) {
			_corners[i]->edges[j] = _edges[_corners[i]->edgeIDs[j]].get();
		}

		for (int j = 0 ; j < _corners[i]->cornerIDs.size() ; j++) {
			_corners[i]->corners[j] = _corners[_corners[i]->cornerIDs[j]].get();
		}
	}
}

void Map::load(std::string path) {
	std::ostringstream xmlPath;
  xmlPath << path << "map.xml";

  TiXmlDocument doc;
	doc.Parse(ut::textFileToString(xmlPath.str()).c_str());

  TiXmlHandle hDoc(&doc);
	TiXmlElement *elem;
	TiXmlHandle hRoot(0);

	elem = hDoc.FirstChildElement().Element();
	if (!elem) return;

	int size;
	elem->QueryIntAttribute("size", &size);

	_centers.resize(size);

	hRoot = TiXmlHandle(elem);

	loadCenters(hRoot);
	loadEdges(hRoot);
	loadCorners(hRoot);

	// Convert to engine coordinates

	#pragma omp parallel for
	for (int i = 0 ; i < _centers.size() ; i++) {
		_centers[i]->x *= MAX_COORD / MAP_MAX_COORD;
		_centers[i]->y *= MAX_COORD / MAP_MAX_COORD;
	}

	#pragma omp parallel for
	for (int i = 0 ; i < _edges.size() ; i++) {
		if (!_edges[i]->mapEdge) {
			_edges[i]->x *= MAX_COORD / MAP_MAX_COORD;
			_edges[i]->y *= MAX_COORD / MAP_MAX_COORD;
		}
	}

	#pragma omp parallel for
	for (int i = 0 ; i < _corners.size() ; i++) {
		_corners[i]->x *= MAX_COORD / MAP_MAX_COORD;
		_corners[i]->y *= MAX_COORD / MAP_MAX_COORD;
	}

	setPointersInDataStructures();
}

void Map::feedGeometryData(TerrainGeometry& terrainGeometry) const {
	TerrainGeometry::SubdivisionLevel* initTerrainGeometry = terrainGeometry.getFirstSubdivLevel();

	initTerrainGeometry->goingToAddNPoints(_corners.size()+_centers.size());

	for (auto ctr = _centers.begin(); ctr != _centers.end(); ctr++) {
		bool toDraw = true;

		if ((*ctr)->biome == Biome::OCEAN) {
			toDraw = false;
			// We still draw an OCEAN biome if one of its neigbours is not an OCEAN
			for (auto edge = (*ctr)->edges.begin(); edge != (*ctr)->edges.end(); edge++) {
				if (!(*edge)->mapEdge && (*edge)->center0->biome != (*edge)->center1->biome) {
					toDraw = true;
					break;
				}
			}
		}

		if (toDraw) {
			for (auto edge = (*ctr)->edges.begin(); edge != (*ctr)->edges.end(); edge++) {
				if (!(*edge)->mapEdge) {
					std::array<glm::vec3, 3> points;

					points[0].x = (*edge)->corner0->x;
					points[0].y = (*edge)->corner0->y;
					points[0].z = (*edge)->corner0->elevation;

					points[1].x = (*edge)->corner1->x;
					points[1].y = (*edge)->corner1->y;
					points[1].z = (*edge)->corner1->elevation;

					points[2].x = (*ctr)->x;
					points[2].y = (*ctr)->y;
					points[2].z = (*ctr)->elevation;

					initTerrainGeometry->addTriangle(points, (*ctr)->biome);
				}
			}
		}
	}

	initTerrainGeometry->computeNormals();
}
