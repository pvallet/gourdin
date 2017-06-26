#pragma once

#include <GL/glew.h>
#include "opengl.h"

#include <vector>

#include "igElement.h"

#define IGE_BUFFER_SIZE 10000

enum DrawType {STATIC_DRAW, STREAM_DRAW};

class igElementDisplay {
public:
  igElementDisplay();
  virtual ~igElementDisplay();
  igElementDisplay(igElementDisplay const&) = delete;
  void operator=  (igElementDisplay const&) = delete;

  void init(DrawType drawType = STREAM_DRAW, size_t capacity = IGE_BUFFER_SIZE);

  void loadElements(const std::vector<igElement*>& visibleElmts);
  size_t drawElements() const;

protected:
  void processSpree(const std::vector<igElement*>& visibleElmts,
    size_t& currentSpreeLength, size_t& firstIndexSpree);

  void reset();

  size_t _capacity;

  GLuint _vao;
  GLuint _vbo;
  GLuint _ibo;
  std::vector<GLuint> _texIDs;
  std::vector<GLuint> _nbElemsInSpree;
};
