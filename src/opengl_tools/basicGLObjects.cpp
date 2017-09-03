#include "basicGLObjects.h"

VertexArrayObject::VertexArrayObject():
  _vaoID(0) {
  glGenVertexArrays(1, &_vaoID);
}

void VertexArrayObject::generate() {
  glDeleteVertexArrays(1, &_vaoID);
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

void VertexBufferObject::generate() {
  glDeleteBuffers(1, &_vboID);
  glGenBuffers(1, &_vboID);
}

VertexBufferObject::~VertexBufferObject() {
  glDeleteBuffers(1, &_vboID);
}

VertexBufferObject::VertexBufferObject (VertexBufferObject&& other) noexcept:
  _vboID(other._vboID) {
  other._vboID = 0;
}

void VertexBufferObject::cpuBufferSubData(std::vector<float>& bufferData, size_t offset, size_t size, const void* data) {
  const float* floatData = (const float*) data;

  for (size_t i = 0; i < size; i++) {
    bufferData[i + offset] = floatData[i];
  }
}


IndexBufferObject::IndexBufferObject():
  _iboID(0) {
  glGenBuffers(1, &_iboID);
}

void IndexBufferObject::generate() {
  glDeleteBuffers(1, &_iboID);
  glGenBuffers(1, &_iboID);
}

IndexBufferObject::~IndexBufferObject() {
  glDeleteBuffers(1, &_iboID);
}

IndexBufferObject::IndexBufferObject (IndexBufferObject&& other) noexcept:
  _iboID(other._iboID) {
  other._iboID = 0;
}
