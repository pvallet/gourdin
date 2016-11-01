#include "chunk.h"

#include "camera.h"
#include "vecUtils.h"

Chunk::Chunk(size_t x, size_t y) :
	_chunkPos(x,y),
	_visible(false) {
	glGenBuffers(1, &_vbo);
	glGenBuffers(1, &_ibo);
}

Chunk::~Chunk() {
	glDeleteBuffers(1, &_vbo);
	glDeleteBuffers(1, &_ibo);
}

int Chunk::compareToPoints(sf::Vector3f cam, sf::Vector3f vec, sf::Vector3f* points) const {
    float dots[4];

    for (size_t i = 0 ; i < 4 ; i++) {
        dots[i] = vu::dot(points[i]-cam, vec);
    }

    if (dots[0] >= 0.f && dots[1] >= 0.f &&
      	dots[2] >= 0.f && dots[3] >= 0.f )
      return 1;

    else if (dots[0] <= 0.f && dots[1] <= 0.f &&
             dots[2] <= 0.f && dots[3] <= 0.f )
      return -1;

    else
      return 0;
}

void Chunk::computeCulling() {
	Camera& cam = Camera::getInstance();

  float theta = cam.getTheta();
  float phi   = cam.getPhi();
  float alpha = cam.getFov() * cam.getRatio() / 2.f;

  // Bottom of the view
  sf::Vector3f norm = vu::carthesian(1.f, theta, phi + 90.f - cam.getFov() / 2.f);
  sf::Vector3f pos = cam.getPos();

  if (compareToPoints(pos,norm,_corners) == 1) {
    _visible = false;
    return;
  }

  // Top
  norm = vu::carthesian(1.f, theta, phi + 90.f + cam.getFov() / 2.f);
  norm *= -1.f;

  if (compareToPoints(pos,norm,_corners) == 1) {
    _visible = false;
    return;
  }

  // Right
  norm = vu::carthesian(1.f, theta + 90.f, 90.f);
  vu::Mat3f rot;
  rot.rotation(vu::carthesian(1.f, theta + 180.f, 90.f - phi), - alpha);
  norm = rot.multiply(norm);

  if (compareToPoints(pos,norm,_corners) == 1) {
    _visible = false;
    return;
  }

  // Left
  norm = vu::carthesian(1.f, theta - 90.f, 90.f);
  rot.rotation(vu::carthesian(1.f, theta + 180.f, 90.f - phi), alpha);
  norm = rot.multiply(norm);

  if (compareToPoints(pos,norm,_corners) == 1) {
    _visible = false;
    return;
  }

  _visible = true;
}
