#include "igElementDisplay.h"

#include <cmath>

#include "igMovingElement.h"
#include "tree.h"
#include "utils.h"

#define IGE_BUFFER_SIZE 20000

igElementDisplay::igElementDisplay() :
  _vao(0),
  _vbo(0) {

  glGenBuffers(1, &_vbo);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  // 24 is 12 for vertices, 8 for texture coordinates and 4 for texture layer
  glBufferData(GL_ARRAY_BUFFER, IGE_BUFFER_SIZE * 24 * sizeof(float),
    NULL, GL_STREAM_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glGenVertexArrays(1, &_vao);
  glBindVertexArray(_vao);
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);

  size_t sizeVertices = IGE_BUFFER_SIZE * 12 * sizeof(float);
  size_t sizeCoord2D  = IGE_BUFFER_SIZE *  8 * sizeof(float);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeVertices));
  glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeVertices + sizeCoord2D));

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);
}

igElementDisplay::~igElementDisplay() {
  glDeleteBuffers(1, &_vbo);
  glDeleteVertexArrays(1, &_vao);
}

void igElementDisplay::processSpree(const std::vector<igElement*>& elemsToDisplay,
  size_t& currentSpreeLength, size_t& firstIndexSpree) {

  if (currentSpreeLength != 0) { // otherwise first call, there is no spree yet

    _texIDs.push_back(elemsToDisplay[firstIndexSpree]->getTexID());
    _nbElemsInSpree.push_back(currentSpreeLength);

    for (size_t i = firstIndexSpree; i < firstIndexSpree + currentSpreeLength; i++) {
      std::array<float, 12> vertices;
      std::array<float,  8> coord2D;
      std::array<float,  4> layer;

      if (i < elemsToDisplay.size()) {
        vertices = elemsToDisplay[i]->getVertices();
        coord2D  = elemsToDisplay[i]->getCoord2D();
        layer    = elemsToDisplay[i]->getLayer();
      }

      glBufferSubData(GL_ARRAY_BUFFER,
        i*sizeof(vertices),
    		sizeof(vertices), &vertices[0]);

      glBufferSubData(GL_ARRAY_BUFFER,
        IGE_BUFFER_SIZE*sizeof(vertices) + i*sizeof(coord2D),
        sizeof(coord2D), &coord2D[0]);

      glBufferSubData(GL_ARRAY_BUFFER,
        IGE_BUFFER_SIZE*(sizeof(vertices) + sizeof(coord2D)) + i*sizeof(layer),
    		sizeof(layer), &layer[0]);
    }

    // Update the spree infos for the next spree

    firstIndexSpree += currentSpreeLength;
    currentSpreeLength = 0;
  }
}

enum CurrentType {NO_TYPE, ANIMAL, TREE};

void igElementDisplay::loadElements(const std::vector<igElement*>& visibleElmts) {
  _texIDs.clear();
  _nbElemsInSpree.clear();

  std::vector<igElement*> elemsToDisplay;

  if (visibleElmts.size() > IGE_BUFFER_SIZE)
    elemsToDisplay = std::vector<igElement*>(visibleElmts.end() - IGE_BUFFER_SIZE, visibleElmts.end());

  else
    elemsToDisplay = visibleElmts;

  size_t currentSpreeLength = 0;
  size_t firstIndexSpree = 0;

  CurrentType currentType = NO_TYPE;
  Animals     currentAnimal;
  Biome       currentBiome;

  glBindBuffer(GL_ARRAY_BUFFER, _vbo);

  for (size_t i = 0; i < elemsToDisplay.size(); i++) {
    Tree* tree; igMovingElement* animal;

    if (tree = dynamic_cast<Tree*>(elemsToDisplay[i])) {
      if (currentType != TREE || currentBiome != tree->getBiome()) {
        processSpree(elemsToDisplay, currentSpreeLength, firstIndexSpree);
        currentType  = TREE;
        currentBiome = tree->getBiome();
      }
    }

    else if (animal = dynamic_cast<igMovingElement*>(elemsToDisplay[i])) {
      if (currentType != ANIMAL || currentAnimal != animal->getAnimalType()) {
        processSpree(elemsToDisplay, currentSpreeLength, firstIndexSpree);
        currentType   = ANIMAL;
        currentAnimal = animal->getAnimalType();
      }
    }

    currentSpreeLength++;
  }

  processSpree(elemsToDisplay, currentSpreeLength, firstIndexSpree);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void igElementDisplay::drawElements() const {
  size_t cursor = 0;

  glBindVertexArray(_vao);

  for (size_t i = 0; i < _nbElemsInSpree.size(); i++) {
    glBindTexture(GL_TEXTURE_2D_ARRAY, _texIDs[i]);

    glDrawArrays(GL_QUADS, cursor, 4 * _nbElemsInSpree[i]);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    cursor += 4 * _nbElemsInSpree[i];
  }

  glBindVertexArray(0);
}
