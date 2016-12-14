#include "treeTexManager.h"

#include "tinyxml.h"

#include <iostream>

TreeTexManager::TreeTexManager() :
  _heightFactor(0.15) {}

void TreeTexManager::load(std::string path) {
  Flora defaultF;
  defaultF.biome = NO_DEFINED_BIOME;
  _flora.assign(NB_BIOMES, defaultF);

	std::ostringstream xmlPath;
    xmlPath << path << "trees.xml";

	TiXmlDocument doc(xmlPath.str());
  if(!doc.LoadFile()) {
    std::cerr << "Error while loading file: " << xmlPath.str() << std::endl;
    std::cerr << "Error #" << doc.ErrorId() << ": " << doc.ErrorDesc() << std::endl;
  }

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
  for (size_t i = 0; i < NB_BIOMES; i++) {
    if (_flora[i].biome != NO_DEFINED_BIOME) {
      std::ostringstream subPath;
      subPath << path << currentBiome << '/';

      _flora[i].texArray = _aTexManager.loadTextures(_flora[i].nbTrees, subPath.str());
      currentBiome++;
    }
  }
}
