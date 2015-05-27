#include "camera.h"

#include <SFML/OpenGL.hpp>

Camera::Camera() : 	fovAngle(45.0),
    				aspectRatio(1.0),
    				nearPlane(0.1),
    				farPlane(10000.0),
    				x(0.0), y(0.0),
    				r(100.0),
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
	float rad = M_PI / 180.;

    // y and z are inverted
    pos = sf::Vector3f(x + r*sin(phi*rad)*cos(theta*rad), r*cos(phi*rad), y + r*sin(phi*rad)*sin(theta*rad));

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(	pos.x, pos.y, pos.z,
				x, 0., y,
				0., 1., 0.);
				//r*sin((phi-90.)*rad)*cos(theta*rad), r*sin((phi-90))*rad*sin(theta*rad), r*cos((phi-90.)*rad));
}

