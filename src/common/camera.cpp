#include "camera.h"
#include "vecUtils.h"

Camera::Camera() :
  _fovAngle(45),
	_aspectRatio(1.f),
	_nearPlane(0.1f),
	_farPlane(100000.f),
	_x(0.f), _y(0.f), _height(0.f),
	_r(INIT_R),
	_phi(60.f),
	_theta(0.f),
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
}

void Camera::apply() {
  _pos = sf::Vector3f(_x,_y,_height) + vu::carthesian(_r, _theta, _phi);

  _view = glm::lookAt (
	  vu::convertGLM(_pos),
    glm::vec3(_x, _y, _height),
    vu::convertGLM(vu::carthesian(1.f, _theta, _phi + _fovAngle/2.f - 90.f))
  );

  _viewProjection = _projection * _view;
}
