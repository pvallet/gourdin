#pragma once

#include <SFML/Graphics.hpp>

// From SFML/OpenGL.hpp, adapted for glu.h
#if defined(SFML_SYSTEM_WINDOWS)
    #ifdef _MSC_VER
        #include <windows.h>
    #endif
#endif

#include <GL/glu.h>
#include <cmath>

#define RAD M_PI / 180.

class Camera {
public:
	Camera();
	void resize(unsigned int W, unsigned int H);

	inline void translate (float dx, float dy) {_x += dx*cos((_theta+90.)*RAD)+dy*sin((_theta+90.)*RAD);
																							_y += dx*sin((_theta+90.)*RAD)-dy*cos((_theta+90.)*RAD);}
	inline void rotate (float dtheta, float dphi) {_theta += dtheta; _phi += dphi;}
	inline void zoom (float dr) {_r += dr;}
	void apply ();
  inline void setPointedPos(sf::Vector2f newPos) {_x = newPos.x; _y = newPos.y;}
  inline void setHeight(float nHeight) {_height = nHeight;}

	inline unsigned int getW() const {return _W;}
	inline unsigned int getH() const {return _H;}
  inline sf::Vector3f getPos() const {return _pos;}
  inline float getTheta() const {return _theta;}
  inline float getPhi() const {return _phi;}
  inline float getFov() const {return _fovAngle;}
  inline float getRatio() const {return _aspectRatio;}
  inline sf::Vector2<double> getPointedPos() const {return sf::Vector2<double>(_x, _y);}

private:
	unsigned int _W, _H;

  float _fovAngle;
  float _aspectRatio;
  float _nearPlane;
  float _farPlane;

  double _x, _y, _height;
  float _r, _phi, _theta; // The camera points towards the center of the sphere
  sf::Vector3f _pos;
};
