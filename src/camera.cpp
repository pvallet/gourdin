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
	_theta(0.f) {}

void Camera::resize(unsigned int W, unsigned int H) {
	_W = W;
	_H = H;

	glViewport (0, 0, (GLint)W, (GLint)H);
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  _aspectRatio = static_cast<float>(W)/static_cast<float>(H);
  gluPerspective (_fovAngle, _aspectRatio, _nearPlane, _farPlane);
  glMatrixMode (GL_MODELVIEW);
}

void Camera::apply() {
  sf::Vector3f tmp = sf::Vector3f(_x,_y,_height) + vu::carthesian(_r, _theta, _phi);
  // y and z are inverted
  _pos.x = tmp.x;
  _pos.y = tmp.z;
  _pos.z = tmp.y;

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(	_pos.x, _pos.y, _pos.z,
      				_x, _height, _y,
      				0., 1., 0.);
      				//_r*sin((_phi-90.)*rad)*cos(_theta*rad), _r*sin((_phi-90))*rad*sin(_theta*rad), _r*cos((_phi-90.)*rad));
}
