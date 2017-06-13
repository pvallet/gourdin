#include "vecUtils.h"

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

float vu::angle(const glm::vec2& u, const glm::vec2& v) { // returns -1 if one is the vector (0,0), else value in degrees
	int sign = u.x * v.y - u.y * v.x >= 0.f ? 1 : -1; // Cross >= 0
  float lengths = glm::length(u)*glm::length(v);

  if (lengths == 0.f) {
    return -1.f;
  }

  else {
  	float nDot = glm::dot(u,v) / lengths;

    return sign * acos(nDot) / RAD;
  }
}

float vu::absoluteAngle(const glm::vec3& u, const glm::vec3& v) {
	float lengths = glm::length(u)*glm::length(v);

  if (lengths == 0.f) {
    return 0.f;
  }

	else {
  	float nDot = glm::dot(u,v) / lengths;

    return acos(nDot) / RAD;
  }
}
