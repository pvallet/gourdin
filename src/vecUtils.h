#pragma once

#include <SFML/Graphics.hpp>
#include <cmath>

namespace vu {

static float norm(const sf::Vector2f& v) {
	return sqrt(v.x*v.x + v.y*v.y);
}

static float dot(const sf::Vector2f& u, const sf::Vector2f& v) {
	return u.x*v.x + u.y*v.y;
}

static float angle(const sf::Vector2f& u, const sf::Vector2f& v) { // returns -1 if one is the vector (0,0), else value in degrees
	int sign = u.x * v.y - u.y * v.x >= 0. ? 1 : -1; // Cross >= 0
    float lengths = norm(u)*norm(v);

    if (lengths == 0.) {
        return -1.f;
    }

    else {
    	float nDot = dot(u,v) / lengths;

        return sign * acos(nDot) * 180. / M_PI;
    }
}

}
