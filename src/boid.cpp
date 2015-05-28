#include "boid.h"

Boid::Boid(sf::Vector2f position, std::string filename) :
	Controllable(position, filename),
	repulsionRadius(10.),
	orientationRadius(30.),
	attractionRadius(50.) {
}
