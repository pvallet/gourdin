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
    float rad = M_PI / 180.f;

    u.x = r*sin(phi*rad)*cos(theta*rad);
    u.y = r*sin(phi*rad)*sin(theta*rad);
    u.z = r*cos(phi*rad);

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

    return sign * acos(nDot) * 180.f / M_PI;
  }
}

float vu::absoluteAngle(const sf::Vector3f& u, const sf::Vector3f& v) {
	float lengths = norm(u)*norm(v);

  if (lengths == 0.f) {
    return 0.f;
  }

	else {
  	float nDot = dot(u,v) / lengths;

    return acos(nDot) * 180.f / M_PI;
  }
}
