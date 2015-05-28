#pragma once

#include "controllable.h"

class Hunter : public Controllable {
public:
	Hunter(sf::Vector2f position, std::string filename);

	virtual MovingType getMovingType() const {return HUNTER;}

};

