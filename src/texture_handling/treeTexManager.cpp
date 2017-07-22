#include "treeTexManager.h"

#include "tinyxml.h"

#include <iostream>

TreeTexManager::TreeTexManager() :
  _heightFactor(0.15) {}

void TreeTexManager::load(std::string path) {
  _flora.resize(BIOME_NB_ITEMS);

  for (size_t i = 0; i < _flora.size(); i++) {
    _flora[i].biome = BIOME_NB_ITEMS;
  }

	std::ostringstream xmlPath;
    xmlPath << path << "trees.xml";

  TiXmlDocument doc;
	doc.Parse(ut::textFileToString(xmlPath.str()).c_str());

	TiXmlHandle hdl(&doc);
  TiXmlElement *elem = hdl.FirstChildElement().FirstChildElement().Element();

  for ( ; elem; elem = elem->NextSiblingElement()) {
    int biome;
    elem->QueryIntAttribute("biome", &biome);
    elem->QueryIntAttribute("nbTrees", &_flora[biome].nbTrees);

    _flora[biome].biome = (Biome) biome;

    elem->QueryIntAttribute("density", &_flora[biome].density);

  }

  size_t currentBiome = 0;
  for (size_t i = 0; i < BIOME_NB_ITEMS; i++) {
    if (_flora[i].biome != BIOME_NB_ITEMS) {
      std::ostringstream subPath;
      subPath << path << currentBiome << '/';

      _flora[i].texArray.loadTextures(_flora[i].nbTrees, subPath.str());
      currentBiome++;
    }
  }
}
