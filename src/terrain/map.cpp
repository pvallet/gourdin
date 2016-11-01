#include "map.h"

#include <omp.h>
#include "tinyxml.h"

#include <iostream>
#include <sstream>


// The coordinates system in the XML file ends at MAP_MAX_COORD
#define MAP_MAX_COORD 600.f

float Edge::getDistanceToEdge(sf::Vector2f pos) {
	sf::Vector2f a(corner1->x - corner0->x, corner1->y - corner0->y);
  sf::Vector2f b(pos.x      - corner0->x, pos.y      - corner0->y);

  float ab = vu::dot(a,b);

  if (ab <= 0)
    return vu::norm(b);
  else {
    float aa = vu::dot(a,a);
    if (ab < aa) {
      sf::Vector2f g = b - ab/aa * a;
      return vu::norm(g);
    }
    else
      return vu::norm(sf::Vector2f(pos.x - corner1->x, pos.y - corner1->y));
  }
}

bool Edge::isOnSameSideAsCenter0(sf::Vector2f pos) const {
	return (vu::dot(normalToCenter0, pos-sf::Vector2f(corner0->x, corner0->y)) >= 0);
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
	for (unsigned int i = 0 ; i < _centers.size() ; i++) {
		for (unsigned int j = 0 ; j < _centers[i]->centerIDs.size() ; j++) {
			_centers[i]->centers[j] = _centers[_centers[i]->centerIDs[j]].get();
		}

		for (unsigned int j = 0 ; j < _centers[i]->edgeIDs.size() ; j++) {
			_centers[i]->edges[j] = _edges[_centers[i]->edgeIDs[j]].get();
		}

		for (unsigned int j = 0 ; j < _centers[i]->cornerIDs.size() ; j++) {
			_centers[i]->corners[j] = _corners[_centers[i]->cornerIDs[j]].get();
		}
	}

	#pragma omp parallel for
	for (unsigned int i = 0 ; i < _edges.size() ; i++) {
		if (!_edges[i]->mapEdge) {
			_edges[i]->center0 = _centers[_edges[i]->center0ID].get();
			_edges[i]->center1 = _centers[_edges[i]->center1ID].get();
			_edges[i]->corner0 = _corners[_edges[i]->corner0ID].get();
			_edges[i]->corner1 = _corners[_edges[i]->corner1ID].get();
		}
	}

	#pragma omp parallel for
	for (unsigned int i = 0 ; i < _corners.size() ; i++) {
		for (unsigned int j = 0 ; j < _corners[i]->centerIDs.size() ; j++) {
			_corners[i]->centers[j] = _centers[_corners[i]->centerIDs[j]].get();
		}

		for (unsigned int j = 0 ; j < _corners[i]->edgeIDs.size() ; j++) {
			_corners[i]->edges[j] = _edges[_corners[i]->edgeIDs[j]].get();
		}

		for (unsigned int j = 0 ; j < _corners[i]->cornerIDs.size() ; j++) {
			_corners[i]->corners[j] = _corners[_corners[i]->cornerIDs[j]].get();
		}
	}
}

void Map::sortCenters(float tolerance) {
	_centersInChunks.resize(NB_CHUNKS*NB_CHUNKS);

	for (auto it = _centersInChunks.begin(); it != _centersInChunks.end(); it++) {
		it->isOcean = true;
	}

	for (size_t i = 0; i < _centers.size(); i++) {
		for (size_t j = std::max(0,         (int) ((_centers[i]->x - tolerance) / CHUNK_SIZE));
								j < std::min(NB_CHUNKS, (int) ((_centers[i]->x + tolerance) / CHUNK_SIZE) + 1); j++) {
		for (size_t k = std::max(0,         (int) ((_centers[i]->y - tolerance) / CHUNK_SIZE));
								k < std::min(NB_CHUNKS, (int) ((_centers[i]->y + tolerance) / CHUNK_SIZE) + 1); k++) {

			_centersInChunks[j*NB_CHUNKS + k].centers.push_back(_centers[i].get());

			if (_centers[i]->biome != OCEAN)
				_centersInChunks[j*NB_CHUNKS + k].isOcean = false;
		}
		}
	}

	// Initialize the kdIndex to compute the nearest center for each chunk
	for (auto it = _centersInChunks.begin(); it != _centersInChunks.end(); it++) {
		it->data.resize(it->centers.size()*2);

		#pragma omp parallel for
		for (unsigned int i = 0 ; i < it->centers.size() ; i++) {
			it->data[2*i]   = it->centers[i]->x;
			it->data[2*i+1] = it->centers[i]->y;
		}

		it->dataset = flann::Matrix<float>(&(it->data[0]), it->centers.size(), 2);

		it->kdIndex = std::unique_ptr<flann::Index<flann::L2<float> > >(
			new flann::Index<flann::L2<float> >(it->dataset, flann::KDTreeIndexParams(4)));

		it->kdIndex->buildIndex();
	}
}

void Map::computeEdgeBoundingBoxes() {
	for (size_t i = 0; i < _edges.size(); i++) {
		if (!_edges[i]->mapEdge) {
			_edges[i]->beginX = std::min(_edges[i]->corner0->x, _edges[i]->corner1->x) - TERRAIN_TEX_TRANSITION_SIZE;
			_edges[i]->endX   = std::max(_edges[i]->corner0->x, _edges[i]->corner1->x) + TERRAIN_TEX_TRANSITION_SIZE;
			_edges[i]->beginY = std::min(_edges[i]->corner0->y, _edges[i]->corner1->y) - TERRAIN_TEX_TRANSITION_SIZE;
			_edges[i]->endY   = std::max(_edges[i]->corner0->y, _edges[i]->corner1->y) + TERRAIN_TEX_TRANSITION_SIZE;

			sf::Vector2f edge(     _edges[i]->corner1->x - _edges[i]->corner0->x,
				                     _edges[i]->corner1->y - _edges[i]->corner0->y);
			sf::Vector2f toCenter0(_edges[i]->center0->x - _edges[i]->corner0->x,
				                     _edges[i]->center0->y - _edges[i]->corner0->y);

			sf::Vector2f project = (vu::dot(edge,toCenter0)/(float) pow(vu::norm(edge),2)) * edge;

			_edges[i]->normalToCenter0 = toCenter0 - project;
		}
	}

}

void Map::sortEdges() {
	_edgesInChunks.resize(NB_CHUNKS*NB_CHUNKS);

	for (auto e = _edges.begin(); e != _edges.end(); e++) {
		if (!(*e)->mapEdge) {
			// We only need the edges that are a border between two different biomes
			if ((*e)->center0->biome != (*e)->center1->biome) {
				for (size_t i = std::max(0,         (int) ((*e)->beginX / CHUNK_SIZE));
										i < std::min(NB_CHUNKS, (int) ((*e)->endX   / CHUNK_SIZE) + 1); i++) {
				for (size_t j = std::max(0,         (int) ((*e)->beginY / CHUNK_SIZE));
										j < std::min(NB_CHUNKS, (int) ((*e)->endY   / CHUNK_SIZE) + 1); j++) {

					_edgesInChunks[i*NB_CHUNKS + j].insert(e->get());
				}
				}
			}
		}
	}
}

void Map::load(std::string path) {
	std::ostringstream xmlPath;
  xmlPath << path << "map.xml";

  TiXmlDocument doc(xmlPath.str());
  if(!doc.LoadFile()) {
    std::cerr << "Error while loading file: " << xmlPath.str() << std::endl;
    std::cerr << "Error #" << doc.ErrorId() << ": " << doc.ErrorDesc() << std::endl;
  }

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

	// Convert to game coordinates

	#pragma omp parallel for
	for (unsigned int i = 0 ; i < _centers.size() ; i++) {
		_centers[i]->x *= MAX_COORD / MAP_MAX_COORD;
		_centers[i]->y *= MAX_COORD / MAP_MAX_COORD;
	}

	#pragma omp parallel for
	for (unsigned int i = 0 ; i < _edges.size() ; i++) {
		if (!_edges[i]->mapEdge) {
			_edges[i]->x *= MAX_COORD / MAP_MAX_COORD;
			_edges[i]->y *= MAX_COORD / MAP_MAX_COORD;
		}
	}

	#pragma omp parallel for
	for (unsigned int i = 0 ; i < _corners.size() ; i++) {
		_corners[i]->x *= MAX_COORD / MAP_MAX_COORD;
		_corners[i]->y *= MAX_COORD / MAP_MAX_COORD;
	}

	setPointersInDataStructures();

	sortCenters(1.2*CHUNK_SIZE);
	computeEdgeBoundingBoxes();
	sortEdges();
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

Center* Map::getClosestCenter(sf::Vector2f pos) const {
	float queryData[2];
	queryData[0] = pos.x;
	queryData[1] = pos.y;

	flann::Matrix<float> query(queryData, 1, 2);

  std::vector<std::vector<int> > indices;
  std::vector<std::vector<float> > dists;

	sf::Vector2i chunkPos(pos.x / CHUNK_SIZE, pos.y / CHUNK_SIZE);
	size_t index = chunkPos.x * NB_CHUNKS + chunkPos.y;

  _centersInChunks[index].kdIndex->knnSearch(
		query, indices, dists, 1, flann::SearchParams(_centersInChunks[index].centers.size()));

	return _centersInChunks[index].centers[indices[0][0]];
}

std::vector<Center*> Map::getCentersInChunk(size_t x, size_t y) const {

  std::vector<Center*> res;
  for (auto it  = _centersInChunks[x*NB_CHUNKS + y].centers.begin();
						it != _centersInChunks[x*NB_CHUNKS + y].centers.end(); it++) {

  	if ((*it)->x >=     x * CHUNK_SIZE &&
	  		(*it)->x <= (x+1) * CHUNK_SIZE &&
	  		(*it)->y >=     y * CHUNK_SIZE &&
	  		(*it)->y <= (y+1) * CHUNK_SIZE) {

  		res.push_back((*it));
  	}
  }

	return res;
}
