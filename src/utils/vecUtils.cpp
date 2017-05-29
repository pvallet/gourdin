#include "vecUtils.h"

float vu::norm(const sf::Vector2f& v) {
	return sqrt(v.x*v.x + v.y*v.y);
}

float vu::norm(const sf::Vector2i& v) {
	return sqrt(v.x*v.x + v.y*v.y);
}

float vu::norm(const sf::Vector3f& v) {
  return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

float vu::dot(const sf::Vector2f& u, const sf::Vector2f& v) {
	return u.x*v.x + u.y*v.y;
}

float vu::dot(const sf::Vector3f& u, const sf::Vector3f& v) {
  return u.x*v.x + u.y*v.y + u.z*v.z;
}

sf::Vector3f vu::cross(const sf::Vector3f& u, const sf::Vector3f& v) {
  return sf::Vector3f(u.y * v.z - u.z * v.y,
                      u.z * v.x - u.x * v.z,
                      u.x * v.y - u.y * v.x);
}

sf::Vector3f vu::carthesian(float r, float theta, float phi) {
  sf::Vector3f u;

  u.x = r*sin(phi*RAD)*cos(theta*RAD);
  u.y = r*sin(phi*RAD)*sin(theta*RAD);
  u.z = r*cos(phi*RAD);

  return u;
}

sf::Vector3f vu::spherical(float x, float y, float z) {
	sf::Vector3f u(0,0,0);
	u.x = sqrt(x*x + y*y + z*z);
	if (u.x != 0) {
		u.y = atan2(y,x) / RAD;
		if (u.y < 0)
			u.y += 360;

		u.z = acos(z/u.x) / RAD;
	}

	return u;
}

sf::Vector3f vu::convertSFML(glm::vec3 u) {
	sf::Vector3f res;

	res.x = u.x;
	res.y = u.y;
	res.z = u.z;

	return res;
}

glm::vec3 vu::convertGLM(sf::Vector3f u) {
	glm::vec3 res;

	res.x = u.x;
	res.y = u.y;
	res.z = u.z;

	return res;
}

float vu::angle(const sf::Vector2f& u, const sf::Vector2f& v) { // returns -1 if one is the vector (0,0), else value in degrees
	int sign = u.x * v.y - u.y * v.x >= 0.f ? 1 : -1; // Cross >= 0
  float lengths = norm(u)*norm(v);

  if (lengths == 0.f) {
    return -1.f;
  }

  else {
  	float nDot = dot(u,v) / lengths;

    return sign * acos(nDot) / RAD;
  }
}

float vu::absoluteAngle(const sf::Vector3f& u, const sf::Vector3f& v) {
	float lengths = norm(u)*norm(v);

  if (lengths == 0.f) {
    return 0.f;
  }

	else {
  	float nDot = dot(u,v) / lengths;

    return acos(nDot) / RAD;
  }
}
