#include "treeTexManager.h"

#include <iostream>
#include <tinyxml.h>

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
    elem->QueryIntAttribute("extension", &flr.extension);

    _flora[flr.biome] = Flora(flr);
  }

 	size_t currentIndex = 0;
  for (unsigned int i = 0 ; i < NB_BIOMES ; i++) {
    _flora[i].texIndices.clear();
    if (_flora[i].biome != NO_DEFINED_BIOME) {
      for (int j = 0 ; j < _flora[i].nbTrees ; j++) {
       	std::ostringstream treePath;
	      treePath << path << i << "_" << j << ".png";

        sf::Vector2u sizeU = loadTexture(treePath.str());
        _flora[i].size.push_back(sf::Vector2f(sizeU.x,sizeU.y));

        _flora[i].texIndices.push_back(currentIndex);
        currentIndex++;
      }
    }
  }
}
