#pragma once

#include "basicGLObjects.h"
#include "texArray.h"

#include <stddef.h> // size_t
#include <vector>

#include "igElement.h"

class igElementDisplay {
public:
  igElementDisplay() {}

  void loadElements(const std::vector<igElement*>& visibleElmts, bool onlyOnce = false);
  size_t drawElements() const;

protected:
  void fillBufferData(GLenum drawType);
  void processSpree(const std::vector<igElement*>& visibleElmts,
    size_t& currentSpreeLength, size_t& firstIndexSpree);

  void reset();

  size_t _capacity = 0;
  bool _fixedCapacity = false;

  std::vector<float> _data;

  VertexArrayObject _vao;
  VertexBufferObject _vbo;
  IndexBufferObject _ibo;
  std::vector<const TextureArray*> _textures;
  std::vector<size_t> _nbElemsInSpree;
};
