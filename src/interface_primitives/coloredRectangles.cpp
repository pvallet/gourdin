#include "coloredRectangles.h"
#include "coordConversion.h"

Shader ColoredRectangles::_plainColorShader;
bool ColoredRectangles::_shaderLoaded = false;

ColoredRectangles::ColoredRectangles (glm::vec4 color, bool filled):
	_linesOffset(1e-5),
	_color(color),
	_filled(filled),
  _nbRect(0) {

	loadShader();

	_vao.bind();
	_vbo.bind();

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);

	VertexBufferObject::unbind();
	VertexArrayObject::unbind();
}

ColoredRectangles::ColoredRectangles (glm::vec4 color, const glm::ivec4& rectangle, bool filled):
 ColoredRectangles(color, std::vector<glm::ivec4>(1,rectangle), filled) {}

ColoredRectangles::ColoredRectangles (glm::vec4 color, const std::vector<glm::ivec4>& rectangles, bool filled):
  ColoredRectangles(color, filled) {
  setRectangles(rectangles);
}

void ColoredRectangles::loadShader() {
  if (!ColoredRectangles::_shaderLoaded) {
    ColoredRectangles::_plainColorShader.load("src/shaders/2D_shaders/2D_noTexCoords.vert", "src/shaders/2D_shaders/plainColor.frag");
    ColoredRectangles::_shaderLoaded = true;
  }
}

void ColoredRectangles::setRectangles(const glm::ivec4& rectangle) {
	setRectangles(std::vector<glm::ivec4>(1,rectangle));
}

void ColoredRectangles::setRectangles(const std::vector<glm::ivec4>& rectangles) {
  _vbo.bind();
  _nbRect = rectangles.size();
  size_t rectGLDataSize;

	if (_nbRect == 0)
		return;

	if (_filled)
		rectGLDataSize = 12;
	else
		rectGLDataSize = 16;

	std::vector<float> bufferData(rectGLDataSize * _nbRect);

  for (int i = 0; i < _nbRect; i++) {
		glm::vec4 glRectangle = ut::windowRectCoordsToGLRectCoords(rectangles[i]);
    float x0 = glRectangle.x;
    float y0 = glRectangle.y;
    float x1 = glRectangle.x + glRectangle.z;
    float y1 = glRectangle.y + glRectangle.w;

		if (_filled) {
		 	struct {float x, y;} data[6] = {
	      { x0, y0 }, { x1, y1 }, { x0, y1 },
	      { x0, y0 }, { x1, y0 }, { x1, y1 }
	    };
			VertexBufferObject::cpuBufferSubData(bufferData, i * rectGLDataSize , rectGLDataSize, data);
		}
		else {
			x0 += _linesOffset; y0 += _linesOffset; x1 -= 2*_linesOffset; y1 -= 2*_linesOffset;

			struct {float x, y;} data[8] = {
				{ x0, y0 }, { x0, y1 }, { x0, y1 }, { x1, y1 },
				{ x1, y1 }, { x1, y0 }, { x1, y0 }, { x0, y0 }
			};
			VertexBufferObject::cpuBufferSubData(bufferData, i * rectGLDataSize , rectGLDataSize, data);
		}

  }

	glBufferData(GL_ARRAY_BUFFER, bufferData.size() * sizeof(float), &bufferData[0], GL_DYNAMIC_DRAW);

  VertexBufferObject::unbind();
}

void ColoredRectangles::bindShaderAndDraw() const {
	_vao.bind();

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
	VertexArrayObject::unbind();
}
