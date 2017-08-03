#include "igElementDisplay.h"

#include <cmath>

#include "igMovingElement.h"
#include "tree.h"
#include "utils.h"

void igElementDisplay::init(size_t capacity) {
  _capacity = capacity;

  if (capacity == 0)
    _fixedCapacity = false;
  else {
    _fixedCapacity = true;
    prepareBuffers(GL_STATIC_DRAW);
  }

  _vao.bind();
  _vbo.bind();

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);

  VertexBufferObject::unbind();
  VertexArrayObject::unbind();
}

void igElementDisplay::prepareBuffers(GLenum drawType) {
  // vbo

  _vbo.bind();
  // 36 is 12 for vertices, 12 for posArray, 8 for texture coordinates and 4 for texture layer
  glBufferData(GL_ARRAY_BUFFER, _capacity * 36 * sizeof(float), NULL, drawType);

  VertexBufferObject::unbind();

  // ibo

  _ibo.bind();

  std::vector<GLuint> indices(6*_capacity);

  for (size_t i = 0; i < _capacity; i++) {
    indices[6*i]     = 0 + 4*i;
    indices[6*i + 1] = 1 + 4*i;
    indices[6*i + 2] = 2 + 4*i;
    indices[6*i + 3] = 0 + 4*i;
    indices[6*i + 4] = 2 + 4*i;
    indices[6*i + 5] = 3 + 4*i;
  }

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, _capacity * 6 * sizeof(indices[0]), NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, _capacity * 6 * sizeof(indices[0]), &indices[0]);
  IndexBufferObject::unbind();

  _vao.bind();
  _vbo.bind();

  size_t sizeVertices = _capacity * 12 * sizeof(float);
  size_t sizeCoord2D = _capacity * 8 * sizeof(float);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeVertices));
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(2*sizeVertices));
  glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(2*sizeVertices + sizeCoord2D));

  VertexBufferObject::unbind();
  VertexArrayObject::unbind();
}

void igElementDisplay::processSpree(const std::vector<igElement*>& elemsToDisplay,
  size_t& currentSpreeLength, size_t& firstIndexSpree) {

  if (currentSpreeLength != 0) { // otherwise first call, there is no spree yet

    _textures.push_back(elemsToDisplay[firstIndexSpree]->getTexArray());
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
  _textures.clear();
  _nbElemsInSpree.clear();

  _capacity = visibleElmts.size();
  prepareBuffers(GL_DYNAMIC_DRAW);

  size_t currentSpreeLength = 0;
  size_t firstIndexSpree = 0;

  CurrentType currentType = NO_TYPE;
  const TextureArray* currentTexture;
  Biome       currentBiome;

  _vbo.bind();

  // Clock initTimer;
  // int previousTime = initTimer.getElapsedTime();
  // SDL_Log("spree: %d ms", initTimer.getElapsedTime() - previousTime);
  // previousTime = initTimer.getElapsedTime();

  for (size_t i = 0; i < visibleElmts.size(); i++) {
    Tree* tree = dynamic_cast<Tree*>(visibleElmts[i]);
    igMovingElement* animal = dynamic_cast<igMovingElement*>(visibleElmts[i]);

    if (tree) {
      if (currentType != TREE || currentBiome != tree->getBiome()) {
        processSpree(visibleElmts, currentSpreeLength, firstIndexSpree);
        currentType  = TREE;
        currentBiome = tree->getBiome();
      }
    }

    else if (animal) {
      if (currentType != ANIMAL || currentTexture != animal->getTexArray()) {
        processSpree(visibleElmts, currentSpreeLength, firstIndexSpree);
        currentType   = ANIMAL;
        currentTexture = animal->getTexArray();
      }
    }

    currentSpreeLength++;
  }

  processSpree(visibleElmts, currentSpreeLength, firstIndexSpree);

  VertexBufferObject::unbind();
}

size_t igElementDisplay::drawElements() const {
  size_t cursor = 0;

  _vao.bind();
  _ibo.bind();

  for (size_t i = 0; i < _nbElemsInSpree.size(); i++) {
    _textures[i]->bind();

    glDrawElements(GL_TRIANGLES, 6 * _nbElemsInSpree[i], GL_UNSIGNED_INT, BUFFER_OFFSET(cursor * sizeof(GLuint)));

    TextureArray::unbind();

    cursor += 6 * _nbElemsInSpree[i];
  }

  VertexBufferObject::unbind();
  VertexArrayObject::unbind();

  return cursor / 6;
}
