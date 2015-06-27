#pragma once

#include <SFML/Graphics.hpp>
#include <cmath>

#define RAD M_PI / 180.

class Camera {
public:
	Camera();
	void resize(unsigned int _W, unsigned int _H);
	
	inline void translate (float dx, float dy) {x += dx*cos((theta+90.)*RAD)+dy*sin((theta+90.)*RAD);
												y += dx*sin((theta+90.)*RAD)-dy*cos((theta+90.)*RAD);}
  	inline void rotate (float dtheta, float dphi) {theta += dtheta; phi += dphi;}
  	inline void zoom (float dr) {r += dr;}
  	void apply ();
    inline void setPointedPos(sf::Vector2f newPos) {x = newPos.x; y = newPos.y;}
    inline void setHeight(float nHeight) {height = nHeight;}

  	inline unsigned int getW() const {return W;}
  	inline unsigned int getH() const {return H;}
    inline sf::Vector3f getPos() const {return pos;}
    inline float getTheta() const {return theta;}
    inline float getPhi() const {return phi;}
    inline float getFov() const {return fovAngle;}
    inline float getRatio() const {return aspectRatio;}
    inline sf::Vector2<double> getPointedPos() const {return sf::Vector2<double>(x, y);}

private:
	unsigned int W, H;

    float fovAngle;
    float aspectRatio;
    float nearPlane;
    float farPlane;

    double x, y, height;
    float r, phi, theta; // The camera points towards the center of the sphere
    sf::Vector3f pos;
};

