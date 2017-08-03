#pragma once

#include "basicGLObjects.h"
#include "texArray.h"

#include <vector>

#include "igElement.h"

class igElementDisplay {
public:
  igElementDisplay() {}

  void init(size_t capacity = 0);

  void loadElements(const std::vector<igElement*>& visibleElmts);
  size_t drawElements() const;

protected:
  void prepareBuffers(GLenum drawType);
  void processSpree(const std::vector<igElement*>& visibleElmts,
    size_t& currentSpreeLength, size_t& firstIndexSpree);

  void reset();

  size_t _capacity;
  bool _fixedCapacity;

  VertexArrayObject _vao;
  VertexBufferObject _vbo;
  IndexBufferObject _ibo;
  std::vector<const TextureArray*> _textures;
  std::vector<size_t> _nbElemsInSpree;
};
