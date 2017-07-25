#include "basicGLObjects.h"

VertexArrayObject::VertexArrayObject():
  _vaoID(0) {
  glGenVertexArrays(1, &_vaoID);
}

VertexArrayObject::~VertexArrayObject() {
  glDeleteVertexArrays(1, &_vaoID);
}

VertexArrayObject::VertexArrayObject (VertexArrayObject&& other) noexcept:
  _vaoID(other._vaoID) {
  other._vaoID = 0;
}



VertexBufferObject::VertexBufferObject():
  _vboID(0) {
  glGenBuffers(1, &_vboID);
}

VertexBufferObject::~VertexBufferObject() {
  glDeleteBuffers(1, &_vboID);
}

VertexBufferObject::VertexBufferObject (VertexBufferObject&& other) noexcept:
  _vboID(other._vboID) {
  other._vboID = 0;
}



IndexBufferObject::IndexBufferObject():
  _iboID(0) {
  glGenBuffers(1, &_iboID);
}

IndexBufferObject::~IndexBufferObject() {
  glDeleteBuffers(1, &_iboID);
}

IndexBufferObject::IndexBufferObject (IndexBufferObject&& other) noexcept:
  _iboID(other._iboID) {
  other._iboID = 0;
}
