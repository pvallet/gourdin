#include "terrainTexManager.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>

TerrainTexManager::TerrainTexManager(std::string folder) {
  texIndices = new GLuint[NB_BIOMES];

  // glEnable(GL_TEXTURE_2D);

  for (int i = 0 ; i < NB_BIOMES ; i++) {

  	sf::Image img;

    std::ostringstream convert;

    convert << folder << i << ".png";

    if (!img.loadFromFile(convert.str())) {
      std::cerr << "Unable to open file " << std::endl;
    }

    glGenTextures(1, &texIndices[i]);

    glBindTexture(GL_TEXTURE_2D, texIndices[i]);

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

  glBindTexture(GL_TEXTURE_2D, 0);

  // glDisable(GL_TEXTURE_2D);
}