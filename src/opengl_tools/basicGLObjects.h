// This files contains RAII classes that handle the lifetime of basic OpenGL objects:
// Index Buffer Objects (IBO)
// Vertex Array Objects (VAO)
// Vertex Buffer Objects (VBO)

#pragma once

#include "opengl.h"
#include "glObjectInterface.h"

#include <vector>

class VertexArrayObject : public GLObjectInterface {
public:
  VertexArrayObject ();
  VertexArrayObject (VertexArrayObject&& other) noexcept;
  ~VertexArrayObject ();

  void generate();
  inline void bind() const {glBindVertexArray(_vaoID);}
  inline static void unbind() {glBindVertexArray(0);}

private:
  GLuint _vaoID;
};

class VertexBufferObject : public GLObjectInterface {
public:
  VertexBufferObject ();
  VertexBufferObject (VertexBufferObject&& other) noexcept;
  ~VertexBufferObject ();

  void generate();
  inline void bind() const {glBindBuffer(GL_ARRAY_BUFFER, _vboID);}
  inline static void unbind() {glBindBuffer(GL_ARRAY_BUFFER, 0);}

  // Mimics glBufferSubData on the CPU as OpenGL ES drivers sometimes send the whole buffer each time
  static void cpuBufferSubData(std::vector<float>& bufferData, size_t offset, size_t size, const void* data);

private:
  GLuint _vboID;
};

class IndexBufferObject : public GLObjectInterface {
public:
  IndexBufferObject ();
  IndexBufferObject (IndexBufferObject&& other) noexcept;
  ~IndexBufferObject ();

  void generate();
  inline void bind() const {glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _iboID);}
  inline static void unbind() {glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);}

private:
  GLuint _iboID;
};
