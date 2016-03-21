#include "camera.h"
#include "vecUtils.h"

#include <SFML/OpenGL.hpp>

Camera::Camera() :
  fovAngle(45.f),
	aspectRatio(1.f),
	nearPlane(0.1f),
	farPlane(100000.0),
	x(0.f), y(0.f), height(0.f),
	r(150.0),
	phi(60.f),
	theta(0.f) {}

void Camera::resize(unsigned int _W, unsigned int _H) {
	W = _W;
	H = _H;

	glViewport (0, 0, (GLint)W, (GLint)H);
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  aspectRatio = static_cast<float>(W)/static_cast<float>(H);
  gluPerspective (fovAngle, aspectRatio, nearPlane, farPlane);
  glMatrixMode (GL_MODELVIEW);
}

void Camera::apply() {
  sf::Vector3f tmp = sf::Vector3f(x,y,height) + vu::carthesian(r, theta, phi);
  // y and z are inverted
  pos.x = tmp.x;
  pos.y = tmp.z;
  pos.z = tmp.y;

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(	pos.x, pos.y, pos.z,
      				x, height, y,
      				0., 1., 0.);
      				//r*sin((phi-90.)*rad)*cos(theta*rad), r*sin((phi-90))*rad*sin(theta*rad), r*cos((phi-90.)*rad));
}
