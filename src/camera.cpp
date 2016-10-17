#include "camera.h"
#include "vecUtils.h"

#include <SFML/OpenGL.hpp>

Camera::Camera() :
  _fovAngle(45.f),
	_aspectRatio(1.f),
	_nearPlane(0.1f),
	_farPlane(100000.0),
	_x(0.f), _y(0.f), _height(0.f),
	_r(150.0),
	_phi(60.f),
	_theta(0.f),
  _projection(1.f),
  _view(1.f) {}

void Camera::resize(unsigned int W, unsigned int H) {
	_W = W;
	_H = H;

	glViewport (0, 0, (GLint)W, (GLint)H);
  _aspectRatio = static_cast<float>(W)/static_cast<float>(H);
  _projection = glm::perspective(_fovAngle, _aspectRatio, _nearPlane, _farPlane);

  _viewProjection = _projection * _view;
}

void Camera::apply() {
  sf::Vector3f tmp = sf::Vector3f(_x,_y,_height) + vu::carthesian(_r, _theta, _phi);
  // y and z are inverted
  _pos.x = tmp.x;
  _pos.y = tmp.z;
  _pos.z = tmp.y;

  _view = glm::lookAt (
	  glm::vec3(_pos.x, _pos.y, _pos.z),
    glm::vec3(_x, _height, _y),
    glm::vec3(0., 1., 0.)
  );
  //_r*sin((_phi-90.)*rad)*cos(_theta*rad), _r*sin((_phi-90))*rad*sin(_theta*rad), _r*cos((_phi-90.)*rad));

  _viewProjection = _projection * _view;
}
