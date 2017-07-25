#pragma once

#include "basicGLObjects.h"
#include "texArray.h"

#include <vector>

#include "igElement.h"

#define IGE_BUFFER_SIZE 10000

enum DrawType {STATIC_DRAW, STREAM_DRAW};

class igElementDisplay {
public:
  igElementDisplay() {}

  void init(DrawType drawType = STREAM_DRAW, size_t capacity = IGE_BUFFER_SIZE);

  void loadElements(const std::vector<igElement*>& visibleElmts);
  size_t drawElements() const;

protected:
  void processSpree(const std::vector<igElement*>& visibleElmts,
    size_t& currentSpreeLength, size_t& firstIndexSpree);

  void reset();

  size_t _capacity;

  VertexArrayObject _vao;
  VertexBufferObject _vbo;
  IndexBufferObject _ibo;
  std::vector<const TextureArray*> _textures;
  std::vector<size_t> _nbElemsInSpree;
};
