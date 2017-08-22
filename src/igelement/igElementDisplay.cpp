#include "igElementDisplay.h"

#include <cmath>

#include "igMovingElement.h"
#include "tree.h"
#include "utils.h"

void igElementDisplay::fillBufferData(GLenum drawType) {
  // vbo

  _vbo.bind();
  glBufferData(GL_ARRAY_BUFFER, _data.size() * sizeof(float), &_data[0], drawType);

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

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, _capacity * 6 * sizeof(indices[0]), &indices[0], GL_STATIC_DRAW);
  IndexBufferObject::unbind();

  _vao.bind();
  _vbo.bind();

  size_t sizeVertices = _capacity * 12 * sizeof(float);
  size_t sizeCoord2D = _capacity * 8 * sizeof(float);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeVertices));
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(2*sizeVertices));
  glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(2*sizeVertices + sizeCoord2D));

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);

  VertexBufferObject::unbind();
  VertexArrayObject::unbind();
}

void igElementDisplay::processSpree(const std::vector<igElement*>& elemsToDisplay,
  size_t& currentSpreeLength, size_t& firstIndexSpree) {

  if (currentSpreeLength != 0) { // otherwise first call, there is no spree yet

    _textures.push_back(elemsToDisplay[firstIndexSpree]->getTexArray());
    _nbElemsInSpree.push_back(currentSpreeLength);

    for (size_t i = firstIndexSpree; i < firstIndexSpree + currentSpreeLength; i++) {
      const std::array<float, 12>& vertices = elemsToDisplay[i]->getVertices();
      const std::array<float, 12>& posArray = elemsToDisplay[i]->getPosArray();
      const std::array<float,  8>& coord2D = elemsToDisplay[i]->getCoord2D();
      const std::array<float,  4>& layer = elemsToDisplay[i]->getLayer();

      std::copy(vertices.begin(), vertices.end(), _data.begin() + i*12);
      std::copy(posArray.begin(), posArray.end(), _data.begin() + _capacity*12 + i*12);
      std::copy(coord2D.begin(),  coord2D.end(),  _data.begin() + _capacity*24 + i*8);
      std::copy(layer.begin(),    layer.end(),    _data.begin() + _capacity*32 + i*4);
    }

    // Update the spree infos for the next spree

    firstIndexSpree += currentSpreeLength;
    currentSpreeLength = 0;
  }
}

enum CurrentType {NO_TYPE, ANIMAL, TREE};

void igElementDisplay::loadElements(const std::vector<igElement*>& visibleElmts, bool onlyOnce) {
  _textures.clear();
  _nbElemsInSpree.clear();

  _capacity = visibleElmts.size();
  _data.resize(_capacity * 36);

  size_t currentSpreeLength = 0;
  size_t firstIndexSpree = 0;

  CurrentType currentType = NO_TYPE;
  const TextureArray* currentTexture;
  Biome       currentBiome;

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

  if (onlyOnce)
    fillBufferData(GL_STATIC_DRAW);
  else
    fillBufferData(GL_DYNAMIC_DRAW);
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
