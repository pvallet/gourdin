#include "treeTexManager.h"

#include "tinyxml.h"

#include <iostream>

TreeTexManager::TreeTexManager() :
  _heightFactor(0.15) {}

void TreeTexManager::load(std::string path) {
  Flora defaultF;
  defaultF.biome = BIOME_NB_ITEMS;
  _flora.assign(BIOME_NB_ITEMS, defaultF);

	std::ostringstream xmlPath;
    xmlPath << path << "trees.xml";

  TiXmlDocument doc;
	doc.Parse(ut::textFileToString(xmlPath.str()).c_str());

	TiXmlHandle hdl(&doc);
  TiXmlElement *elem = hdl.FirstChildElement().FirstChildElement().Element();
  Flora flr;

  for ( ; elem; elem = elem->NextSiblingElement()) {
    int biome;
    elem->QueryIntAttribute("biome", &biome);
    elem->QueryIntAttribute("nbTrees", &flr.nbTrees);

    flr.biome = (Biome) biome;

    elem->QueryIntAttribute("density", &flr.density);

    _flora[flr.biome] = Flora(flr);
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
