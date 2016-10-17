#include "treeTexManager.h"
#include <iostream>
#include <tinyxml.h>

TreeTexManager::TreeTexManager(std::string path) :
    _heightFactor(0.15) {
	_flora.resize(NB_BIOMES, NULL);

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

    _flora[flr.biome] = new Flora(flr);
  }

  sf::Color mask(255, 0, 255);

 	sf::Image img;
  for (unsigned int i = 0 ; i < NB_BIOMES ; i++) {
    if (_flora[i] != NULL) {
      for (int j = 0 ; j < _flora[i]->nbTrees ; j++) {
       	std::ostringstream treePath;
	      treePath << path << i << "_" << j << ".png";

        if (!img.loadFromFile(treePath.str())) {
          std::cerr << "Unable to open file: " << treePath.str() << std::endl;
        }


        sf::Texture* curTex = new sf::Texture();

        img.createMaskFromColor(mask);

        curTex->loadFromImage(img);
        //curTex->setSmooth(true);

        _flora[i]->tex.push_back(curTex);
    	}
    }
  }
}

TreeTexManager::~TreeTexManager() {
	for (unsigned int i = 0 ; i < NB_BIOMES ; i++) {
  	for (int j = 0 ; j < _flora[i]->nbTrees ; j++) {
  		delete _flora[i]->tex[j];
  	}
  }
}
