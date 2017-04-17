#include "igElementDisplay.h"

#include <cmath>

#include "igMovingElement.h"
#include "tree.h"
#include "utils.h"

igElementDisplay::igElementDisplay() :
  _vao(0),
  _vbo(0) {}

void igElementDisplay::reset() {
  glDeleteBuffers(1, &_vbo);
  glDeleteVertexArrays(1, &_vao);
  _vbo = 0;
  _vao = 0;
}

void igElementDisplay::init(DrawType drawType, size_t capacity) {

  reset();

  _capacity = capacity;

  glGenBuffers(1, &_vbo);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  // 36 is 12 for vertices, 12 for posArray, 8 for texture coordinates and 4 for texture layer
  if (drawType == STREAM_DRAW)
    glBufferData(GL_ARRAY_BUFFER, _capacity * 36 * sizeof(float),
      NULL, GL_STREAM_DRAW);
  else
    glBufferData(GL_ARRAY_BUFFER, _capacity * 36 * sizeof(float),
      NULL, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glGenVertexArrays(1, &_vao);
  glBindVertexArray(_vao);
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);

  size_t sizeVertices = _capacity * 12 * sizeof(float);
  size_t sizeCoord2D  = _capacity *  8 * sizeof(float);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeVertices));
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(2*sizeVertices));
  glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(2*sizeVertices + sizeCoord2D));

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);
}

igElementDisplay::~igElementDisplay() {
  reset();
}

void igElementDisplay::processSpree(const std::vector<igElement*>& elemsToDisplay,
  size_t& currentSpreeLength, size_t& firstIndexSpree) {

  if (currentSpreeLength != 0) { // otherwise first call, there is no spree yet

    _texIDs.push_back(elemsToDisplay[firstIndexSpree]->getTexID());
    _nbElemsInSpree.push_back(currentSpreeLength);

    for (size_t i = firstIndexSpree; i < firstIndexSpree + currentSpreeLength; i++) {
      std::array<float, 12> vertices;
      std::array<float, 12> posArray;
      std::array<float,  8> coord2D;
      std::array<float,  4> layer;

      if (i < elemsToDisplay.size()) {
        vertices = elemsToDisplay[i]->getVertices();
        posArray = elemsToDisplay[i]->getPosArray();
        coord2D  = elemsToDisplay[i]->getCoord2D();
        layer    = elemsToDisplay[i]->getLayer();
      }

      glBufferSubData(GL_ARRAY_BUFFER,
        i*sizeof(vertices),
    		sizeof(vertices), &vertices[0]);

      glBufferSubData(GL_ARRAY_BUFFER,
        _capacity*sizeof(vertices) + i*sizeof(posArray),
        sizeof(posArray), &posArray[0]);

      glBufferSubData(GL_ARRAY_BUFFER,
        _capacity*2*sizeof(vertices) + i*sizeof(coord2D),
        sizeof(coord2D), &coord2D[0]);

      glBufferSubData(GL_ARRAY_BUFFER,
        _capacity*(2*sizeof(vertices) + sizeof(coord2D)) + i*sizeof(layer),
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

  if (visibleElmts.size() > _capacity)
    elemsToDisplay = std::vector<igElement*>(visibleElmts.end() - _capacity, visibleElmts.end());

  else
    elemsToDisplay = visibleElmts;

  size_t currentSpreeLength = 0;
  size_t firstIndexSpree = 0;

  CurrentType currentType = NO_TYPE;
  GLuint      currentTexture;
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
      if (currentType != ANIMAL || currentTexture != animal->getTexID()) {
        processSpree(elemsToDisplay, currentSpreeLength, firstIndexSpree);
        currentType   = ANIMAL;
        currentTexture = animal->getTexID();
      }
    }

    currentSpreeLength++;
  }

  processSpree(elemsToDisplay, currentSpreeLength, firstIndexSpree);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

size_t igElementDisplay::drawElements() const {
  size_t cursor = 0;

  glBindVertexArray(_vao);

  for (size_t i = 0; i < _nbElemsInSpree.size(); i++) {
    glBindTexture(GL_TEXTURE_2D_ARRAY, _texIDs[i]);

    glDrawArrays(GL_QUADS, cursor, 4 * _nbElemsInSpree[i]);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    cursor += 4 * _nbElemsInSpree[i];
  }

  glBindVertexArray(0);

  return cursor / 4;
}
