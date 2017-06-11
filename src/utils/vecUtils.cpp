#include "vecUtils.h"

float vu::norm(const glm::vec2& v) {
	return sqrt(v.x*v.x + v.y*v.y);
}

float vu::norm(const glm::ivec2& v) {
	return sqrt(v.x*v.x + v.y*v.y);
}

float vu::norm(const glm::vec3& v) {
  return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

float vu::dot(const glm::vec2& u, const glm::vec2& v) {
	return u.x*v.x + u.y*v.y;
}

float vu::dot(const glm::vec3& u, const glm::vec3& v) {
  return u.x*v.x + u.y*v.y + u.z*v.z;
}

glm::vec3 vu::cross(const glm::vec3& u, const glm::vec3& v) {
  return glm::vec3(u.y * v.z - u.z * v.y,
                      u.z * v.x - u.x * v.z,
                      u.x * v.y - u.y * v.x);
}

glm::vec3 vu::carthesian(float r, float theta, float phi) {
  glm::vec3 u;

  u.x = r*sin(phi*RAD)*cos(theta*RAD);
  u.y = r*sin(phi*RAD)*sin(theta*RAD);
  u.z = r*cos(phi*RAD);

  return u;
}

glm::vec3 vu::spherical(float x, float y, float z) {
	glm::vec3 u(0,0,0);
	u.x = sqrt(x*x + y*y + z*z);
	if (u.x != 0) {
		u.y = atan2(y,x) / RAD;
		if (u.y < 0)
			u.y += 360;

		u.z = acos(z/u.x) / RAD;
	}

	return u;
}

glm::vec3 vu::convertSFML(glm::vec3 u) {
	glm::vec3 res;

	res.x = u.x;
	res.y = u.y;
	res.z = u.z;

	return res;
}

glm::vec3 vu::convertGLM(glm::vec3 u) {
	glm::vec3 res;

	res.x = u.x;
	res.y = u.y;
	res.z = u.z;

	return res;
}

float vu::angle(const glm::vec2& u, const glm::vec2& v) { // returns -1 if one is the vector (0,0), else value in degrees
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

float vu::absoluteAngle(const glm::vec3& u, const glm::vec3& v) {
	float lengths = norm(u)*norm(v);

  if (lengths == 0.f) {
    return 0.f;
  }

	else {
  	float nDot = dot(u,v) / lengths;

    return acos(nDot) / RAD;
  }
}
