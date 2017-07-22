#include "coloredRectangles.h"

Shader ColoredRectangles::_plainColorShader;
bool ColoredRectangles::_shaderLoaded = false;

ColoredRectangles::ColoredRectangles (glm::vec4 color):
	_color(color),
  _nbRect(0) {

	glGenBuffers(1, &_vbo);

	glGenVertexArrays(1, &_vao);
  glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

ColoredRectangles::ColoredRectangles (glm::vec4 color, const std::vector<glm::vec4>& rectangles):
  ColoredRectangles(color) {
  setRectangles(rectangles);
}

ColoredRectangles::~ColoredRectangles() {
	glDeleteBuffers(1, &_vbo);
  glDeleteVertexArrays(1, &_vao);
}

void ColoredRectangles::loadShader() {
  if (!ColoredRectangles::_shaderLoaded) {
    ColoredRectangles::_plainColorShader.load("src/shaders/2D_shaders/2D_noTexCoords.vert", "src/shaders/2D_shaders/plainColor.frag");
    ColoredRectangles::_shaderLoaded = true;
  }
}

void ColoredRectangles::setRectangles(const std::vector<glm::vec4>& rectangles) {
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  _nbRect = rectangles.size();
  size_t rectGLDataSize = 12 * sizeof(float);
  glBufferData(GL_ARRAY_BUFFER, _nbRect * rectGLDataSize, NULL, GL_DYNAMIC_DRAW);

  for (size_t i = 0; i < _nbRect; i++) {
    float x0 = rectangles[i].x;
    float y0 = rectangles[i].y;
    float x1 = rectangles[i].x + rectangles[i].z;
    float y1 = rectangles[i].y + rectangles[i].w;

    struct {float x, y;} data[6] = {
      { x0, y0 }, { x1, y1 }, { x0, y1 },
      { x0, y0 }, { x1, y0 }, { x1, y1 }
    };

    glBufferSubData(GL_ARRAY_BUFFER, i * rectGLDataSize , rectGLDataSize, data);
  }
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ColoredRectangles::draw() const {
	glBindVertexArray(_vao);

  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(_plainColorShader.getProgramID());
  glUniform4f(glGetUniformLocation(_plainColorShader.getProgramID(), "color"), _color.x, _color.y, _color.z, _color.w);

  glDrawArrays(GL_TRIANGLES, 0, 6*_nbRect);

  glUseProgram(0);
  glDisable(GL_BLEND);
	glBindVertexArray(0);
}
