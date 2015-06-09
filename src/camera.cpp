#include "camera.h"
#include "vecUtils.h"

#include <SFML/OpenGL.hpp>

Camera::Camera() : 	fovAngle(45.0),
    				aspectRatio(1.0),
    				nearPlane(0.1),
    				farPlane(10000.0),
    				x(0.0), y(0.0),
    				r(150.0),
    				theta(0.0),
    				phi(60.0) {}

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
    sf::Vector3f tmp = sf::Vector3f(x,y,0.) + vu::carthesian(r, theta, phi);
    // y and z are inverted
    pos.x = tmp.x;
    pos.y = tmp.z;
    pos.z = tmp.y;

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(	pos.x, pos.y, pos.z,
				x, 0., y,
				0., 1., 0.);
				//r*sin((phi-90.)*rad)*cos(theta*rad), r*sin((phi-90))*rad*sin(theta*rad), r*cos((phi-90.)*rad));
}

