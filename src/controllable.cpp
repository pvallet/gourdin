#include "controllable.h"

Controllable::Controllable(sf::Vector2f position, std::string filename) :
 	igMovingElement(position, filename) {
	
}

void Controllable::update(sf::Time elapsed, float theta) {
	igMovingElement::update(elapsed, theta);
	
	// The element has gone too far
	if ((target.x - pos.x) * direction.x + (target.y - pos.y) * direction.y <= 0) {
		pos.x = target.x;
		pos.y = target.y;
		stop();
	}
}

void Controllable::setTarget(sf::Vector2f t) {
	moving = true;
	target = t;

	direction = (t-pos);

	// Angle between (1,0) and direction
	int sign = direction.y >= 0. ? 1 : -1; // Cross >= 0
    float length = sqrt(direction.x*direction.x + direction.y*direction.y);

    if (length == 0.) {
        orientation = 0.;
        direction.x = 0.;
        direction.y = 0.;
        graphics.stop();
    }

    else {
    	direction /= length;

        float nOrientation = sign * acos(direction.x) * 180. / M_PI - camOrientation;
        
        igElement::setOrientation(nOrientation);
        graphics.launchAnimation(ANM_WALK, orientation);
    }
}

