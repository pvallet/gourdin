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

        _flora[i]->tex.push_back(0);
        _flora[i]->size.push_back(img.getSize());

        glGenTextures(1, &(_flora[i]->tex.back()));

        glBindTexture(GL_TEXTURE_2D, _flora[i]->tex.back());

        glTexImage2D(   GL_TEXTURE_2D, 0, GL_RGBA,
                        img.getSize().x, img.getSize().y,
                        0,
                        GL_RGBA, GL_UNSIGNED_BYTE, img.getPixelsPtr()
        );

        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    	}
    }
  }

  glBindTexture(GL_TEXTURE_2D, 0);
}

TreeTexManager::~TreeTexManager() {
	for (unsigned int i = 0 ; i < NB_BIOMES ; i++) {
  	// delete _flora[i];
  }
}
