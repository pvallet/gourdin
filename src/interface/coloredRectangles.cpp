#include "coloredRectangles.h"

Shader ColoredRectangles::_plainColorShader;
bool ColoredRectangles::_shaderLoaded = false;

ColoredRectangles::ColoredRectangles (glm::vec4 color, bool filled):
	_linesOffset(1e-5),
	_color(color),
	_filled(filled),
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

ColoredRectangles::ColoredRectangles (glm::vec4 color, const glm::vec4& rectangle, bool filled):
 ColoredRectangles(color, std::vector<glm::vec4>(1,rectangle), filled) {}

ColoredRectangles::ColoredRectangles (glm::vec4 color, const std::vector<glm::vec4>& rectangles, bool filled):
  ColoredRectangles(color, filled) {
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

void ColoredRectangles::setRectangles(const glm::vec4& rectangle) {
	setRectangles(std::vector<glm::vec4>(1,rectangle));
}

void ColoredRectangles::setRectangles(const std::vector<glm::vec4>& rectangles) {
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  _nbRect = rectangles.size();
  size_t rectGLDataSize;

	if (_filled)
		rectGLDataSize = 12 * sizeof(float);
	else
		rectGLDataSize = 16 * sizeof(float);

  glBufferData(GL_ARRAY_BUFFER, _nbRect * rectGLDataSize, NULL, GL_DYNAMIC_DRAW);

  for (size_t i = 0; i < _nbRect; i++) {
    float x0 = rectangles[i].x;
    float y0 = rectangles[i].y;
    float x1 = rectangles[i].x + rectangles[i].z;
    float y1 = rectangles[i].y + rectangles[i].w;

		if (_filled) {
		 	struct {float x, y;} data[6] = {
	      { x0, y0 }, { x1, y1 }, { x0, y1 },
	      { x0, y0 }, { x1, y0 }, { x1, y1 }
	    };
			glBufferSubData(GL_ARRAY_BUFFER, i * rectGLDataSize , rectGLDataSize, data);
		}
		else {
			x0 += _linesOffset; y0 += _linesOffset; x1 -= 2*_linesOffset; y1 -= 2*_linesOffset;

			struct {float x, y;} data[8] = {
				{ x0, y0 }, { x0, y1 }, { x0, y1 }, { x1, y1 },
				{ x1, y1 }, { x1, y0 }, { x1, y0 }, { x0, y0 }
			};
			glBufferSubData(GL_ARRAY_BUFFER, i * rectGLDataSize , rectGLDataSize, data);
		}

  }
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ColoredRectangles::draw() const {
	glBindVertexArray(_vao);

  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	_plainColorShader.bind();
  glUniform4fv(_plainColorShader.getUniformLocation("color"), 1, &_color[0]);

	if (_filled)
  	glDrawArrays(GL_TRIANGLES, 0, 6*_nbRect);
	else
		glDrawArrays(GL_LINES, 0, 8*_nbRect);

  Shader::unbind();
  glDisable(GL_BLEND);
	glBindVertexArray(0);
}
