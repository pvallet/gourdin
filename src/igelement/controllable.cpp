#include "controllable.h"

#include "camera.h"

Controllable::Controllable(glm::vec2 position, AnimationManager graphics, const TerrainGeometry& terrainGeometry) :
 	igMovingElement(position, graphics, terrainGeometry),
  _target(position) {

}

void Controllable::update(int msElapsed) {
  if (!_dead) {
    igMovingElement::update(msElapsed);
    // The element has gone too far
    if (glm::dot((_target - _pos), getDirection()) < 0) {
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

glm::uvec4 Controllable::getScreenRect() const {
	Camera& cam = Camera::getInstance();

	glm::uvec4 res;

	res.x =  (_projectedVertices[3] + 1.f) / 2.f * cam.getWindowW();
	res.y = -(_projectedVertices[1] + 1.f) / 2.f * cam.getWindowH() + cam.getWindowH();
	res.z =  (_projectedVertices[0] - _projectedVertices[3]) / 2.f * cam.getWindowW();
	res.w =  (_projectedVertices[1] - _projectedVertices[7]) / 2.f * cam.getWindowH();

	return res;
}
