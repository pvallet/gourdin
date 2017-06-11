#include "controllable.h"

#include "camera.h"
#include "vecUtils.h"
#include <iostream>

Controllable::Controllable(glm::vec2 position, AnimationManager graphics, const TerrainGeometry& terrainGeometry) :
 	igMovingElement(position, graphics, terrainGeometry),
  _target(position) {

}

void Controllable::update(sf::Time elapsed) {
  if (!_dead) {
    igMovingElement::update(elapsed);
    // The element has gone too far
    if (vu::dot((_target - _pos), getDirection()) < 0) {
      _pos = _target;
      stop();
    }
  }
}

void Controllable::setTarget(glm::vec2 t, ANM_TYPE anim) {
	if (!_dead) {
		_target = t;
		setDirection(t-_pos);
    launchAnimation(anim);
	}
}

void Controllable::stop() {
  _target = _pos;
  igMovingElement::stop();
}

glm::ivec4 Controllable::getScreenRect() const {
	Camera& cam = Camera::getInstance();

	glm::ivec4 res;

	res.x =  (_projectedVertices[3] + 1.f) / 2.f * cam.getW();
	res.y = -(_projectedVertices[1] + 1.f) / 2.f * cam.getH() + cam.getH();
	res.z =  (_projectedVertices[0] - _projectedVertices[3]) / 2.f * cam.getW();
	res.w =  (_projectedVertices[1] - _projectedVertices[7]) / 2.f * cam.getH();

	return res;
}
