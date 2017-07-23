#include "camera.h"

#include "utils.h"

Camera::Camera() :
  _fovAngle(45),
	_aspectRatio(1.f),
	_nearPlane(1.f),
	_farPlane(100000.f),
  _additionalHeight(0.f),
	_x(0.f), _y(0.f), _height(0.f),
	_r(INIT_R),
	_phi(INIT_PHI),
  _theta(INIT_THETA),
  _projection(1.f),
  _view(1.f) {}

void Camera::resize(unsigned int W, unsigned int H) {
	_W = W;
	_H = H;

	glViewport (0, 0, (GLint)W, (GLint)H);
  _aspectRatio = static_cast<float>(W)/static_cast<float>(H);
  float fovAngleRadians = 45 * M_PI / 180.f;
  _projection = glm::perspective(fovAngleRadians, _aspectRatio, _nearPlane, _farPlane);

  _viewProjection = _projection * _view;
  _skyboxViewProjection = _projection * _skyboxView;
}

void Camera::apply() {
  _pos = glm::vec3(_x,_y,_height) + ut::carthesian(_r, _theta, _phi);

  _view = glm::lookAt (
	  _pos,
    glm::vec3(_x, _y, _height),
    ut::carthesian(1.f, _theta, _phi + _fovAngle/2.f - 90.f)
  );

  _viewProjection = _projection * _view;

  glm::vec3 camDirection = ut::carthesian(1.f, _theta, _phi);

  _skyboxView = glm::lookAt (
	  camDirection,
    glm::vec3(0, 0, 0),
    ut::carthesian(1.f, _theta, _phi + _fovAngle/2.f - 90.f)
  );

  _skyboxViewProjection = _projection * _skyboxView;
}

void Camera::translate(float dWinX, float dWinY) {
  _x += dWinX*cos((_theta+90.f)*RAD)+dWinY*sin((_theta+90.f)*RAD);
  _y +=	dWinX*sin((_theta+90.f)*RAD)-dWinY*cos((_theta+90.f)*RAD);

  if (_x >= MAX_COORD)
    _x = MAX_COORD-1;
  if (_y >= MAX_COORD)
    _y = MAX_COORD-1;
  if (_x < 0)
    _x = 0;
  if (_y < 0)
    _y = 0;
}

void Camera::rotate (float dtheta, float dphi) {
  _theta += dtheta;
  _phi += dphi;

  if (_phi < 0)
    _phi = 0;
  else if (_phi > 180.f)
    _phi = 180.f;
}

glm::vec2 Camera::windowCoordsToGLCoords(glm::uvec2 windowCoords) const {
  return glm::vec2(2 *  windowCoords.x / (float) _windowW - 1,
                   1 - 2 * windowCoords.y / (float) _windowH);
}

glm::uvec2 Camera::glCoordsToWindowCoords(glm::vec2 glCoords) const {
  return glm::uvec2(_windowW * (1 + glCoords.x) / 2,
                    _windowH * (1 - glCoords.y));
}

glm::vec4 Camera::windowRectCoordsToGLRectCoords(glm::uvec4 windowRect) const {
  return glm::vec4(2 *  windowRect.x / (float) _windowW - 1,
                   1 - 2 * (windowRect.y + windowRect.w) / (float) _windowH,
                   2 *  windowRect.z / (float) _windowW,
                   2 *  windowRect.w / (float) _windowH);
}
