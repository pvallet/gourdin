#include "controller.h"
#include "camera.h"

#define ROTATION_ANGLE_PS 60.f // PS = per second

void Controller::handleEventGame(sf::Event event) {

}

void Controller::moveCameraGame() const {
  Camera& cam = Camera::getInstance();

  cam.zoom(-cam.getZoom());

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    cam.rotate(ROTATION_ANGLE_PS * _elapsed.asSeconds(), 0.f);

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    cam.rotate(- ROTATION_ANGLE_PS * _elapsed.asSeconds(), 0.f);
}
