#pragma once

#include <SFML/Graphics.hpp>
#include <cmath>

namespace vu {

float norm(const sf::Vector2f& v);

double norm(const sf::Vector2<double>& v);

float norm(const sf::Vector3f& v);

float dot(const sf::Vector2f& u, const sf::Vector2f& v);

double dot(const sf::Vector2<double>& u, const sf::Vector2<double>& v);

float dot(const sf::Vector3f& u, const sf::Vector3f& v);

sf::Vector3f cross(const sf::Vector3f& u, const sf::Vector3f& v);

sf::Vector3f carthesian(float r, float theta, float phi);

float angle(const sf::Vector2f& u, const sf::Vector2f& v);

double angle(const sf::Vector2<double>& u, const sf::Vector2<double>& v);

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

  inline sf::Vector3f multiply(const sf::Vector3f& v) const
  {
    return sf::Vector3f(m[0]*v.x + m[3]*v.y + m[6]*v.z,
                        m[1]*v.x + m[4]*v.y + m[7]*v.z,
                        m[2]*v.x + m[5]*v.y + m[8]*v.z);
  }

  inline void rotation(sf::Vector3f n, float angle) {
    angle *= M_PI / 180.;
    m[0]=1+(1-cos(angle))*(n.x*n.x-1);            m[3]=-n.z*sin(angle)+(1-cos(angle))*n.x*n.y; m[6]=n.y*sin(angle)+(1-cos(angle))*n.x*n.z;
    m[1]=n.z*sin(angle)+(1-cos(angle))*n.x*n.y;   m[4]=1+(1-cos(angle))*(n.y*n.y-1);           m[7]=-n.x*sin(angle)+(1-cos(angle))*n.y*n.z;
    m[2]=-n.y*sin(angle)+(1-cos(angle))*n.x*n.z;  m[5]=n.x*sin(angle)+(1-cos(angle))*n.y*n.y;  m[8]=1+(1-cos(angle))*(n.z*n.z-1);
  }

  protected:
  float m[9];
};


}
