#pragma once

#include <glm/glm.hpp>

#include "utils.h"

#include <cmath>
#include <map>
#include <memory>

inline bool operator < (const glm::vec3& lhs, const glm::vec3& rhs) {
  if (lhs.x < rhs.x)
    return true;
  else if (lhs.x > rhs.x)
    return false;
  else if (lhs.y < rhs.y)
    return true;
  else if (lhs.y > rhs.y)
    return false;
  else if (lhs.z < rhs.z)
    return true;
  else
    return false;
}

namespace vu {

struct compVec3f {
  bool operator()(const glm::vec3& lhs, const glm::vec3& rhs) const {
    return lhs < rhs;
  }
};

// Rectange a contains vector b
template<typename valType >
bool contains (glm::tvec4< valType > const &a, glm::tvec2< valType > const &b) {
  if ((a.x - b.x) * (a.x + a.z - b.x) < 0 &&
      (a.y - b.y) * (a.y + a.w - b.y) < 0)
    return true;

  return false;
}

glm::vec3 carthesian(float r, float theta, float phi);
glm::vec3 spherical(float x, float y, float z);

inline glm::vec3 carthesian(glm::vec3 u) {return carthesian(u.x,u.y,u.z);}
inline glm::vec3 spherical (glm::vec3 u) {return spherical (u.x,u.y,u.z);}

float angle(const glm::vec2& u, const glm::vec2& v);
float absoluteAngle(const glm::vec3& u, const glm::vec3& v);

class Mat3f {

public:
  inline Mat3f (void)   { m[0] = m[1] = m[2] = m[3] = m[4] = m[5] = m[6] = m[7] = m[8] = 0.0; }

  ~Mat3f() {}

  inline Mat3f (float* mm) {
    m[0] = mm[0];
    m[1] = mm[1];
    m[2] = mm[2];
    m[3] = mm[3];
    m[4] = mm[4];
    m[5] = mm[5];
    m[6] = mm[6];
    m[7] = mm[7];
    m[8] = mm[8];
  };

  inline glm::vec3 multiply(const glm::vec3& v) const
  {
    return glm::vec3(m[0]*v.x + m[3]*v.y + m[6]*v.z,
                        m[1]*v.x + m[4]*v.y + m[7]*v.z,
                        m[2]*v.x + m[5]*v.y + m[8]*v.z);
  }

  inline void rotation(glm::vec3 n, float angle) {
    angle *= M_PI / 180.f;
    m[0]=1+(1-cos(angle))*(n.x*n.x-1);            m[3]=-n.z*sin(angle)+(1-cos(angle))*n.x*n.y; m[6]=n.y*sin(angle)+(1-cos(angle))*n.x*n.z;
    m[1]=n.z*sin(angle)+(1-cos(angle))*n.x*n.y;   m[4]=1+(1-cos(angle))*(n.y*n.y-1);           m[7]=-n.x*sin(angle)+(1-cos(angle))*n.y*n.z;
    m[2]=-n.y*sin(angle)+(1-cos(angle))*n.x*n.z;  m[5]=n.x*sin(angle)+(1-cos(angle))*n.y*n.y;  m[8]=1+(1-cos(angle))*(n.z*n.z-1);
  }

  protected:
  float m[9];
};


}
