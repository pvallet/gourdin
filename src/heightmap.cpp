#include "heightmap.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>

#include "perlin.h"
#include "vecUtils.h"

#define DEFAULT_MAP_SIZE 33 // +1 to join with other chunks
#define BUFFER_OFFSET(a) ((char*)NULL + (a))

#define SMOOTH_RANGE 20. // Percentage


Heightmap::Heightmap(sf::Vector2i chunkPosition, int _seed, sf::Texture* _tex) :
	size(DEFAULT_MAP_SIZE),
	Chunk(chunkPosition),
    seed(_seed)//, tex(_tex) 
    {
	for (unsigned int i = 0 ; i < DEFAULT_MAP_SIZE ; i++) {
		heights.push_back(std::vector<float>(DEFAULT_MAP_SIZE, 0.0f));
	}

	vertices = new float[size*size*3];
	colors = new float[size*size*3];
	indices = new GLuint[2*size*(size-1)];
    coord = new float[2*size*size];
}

void Heightmap::generate(std::vector<Constraint> constraints) {
    Perlin perlin(seed, size);

	for (int i = 0 ; i < size ; i++) {
		for (int j = 0 ; j < size ; j++) {
			heights[i][j] = perlin.getValue(i, j) * HEIGHT_FACTOR;
		}
	}

    float range = size * SMOOTH_RANGE / 100;

    for (unsigned int k = 0 ; k < constraints.size() ; k++) {
        switch(constraints[k].type) {          
            case XN:
                for (int i = 0 ; i < size ; i++) {
                    float dif = constraints[k].vertices[i].y - heights[0][i];

                    for (int j = 0 ; j < (int) range ; j++) {
                        heights[j][i] += dif * (1 - (float) j / range);
                    }
                }
                break;
            
            case XP:
                for (int i = 0 ; i < size ; i++) {
                    float dif = constraints[k].vertices[i].y - heights[size-1][i];
                    
                    for (int j = 0 ; j < (int) range ; j++) {
                        heights[size-1-j][i] += dif * (1 - (float) j / range);
                    }
                }
                break;

            case YN:
                for (int i = 0 ; i < size ; i++) {
                    float dif = constraints[k].vertices[i].y - heights[i][0];

                    for (int j = 0 ; j < (int) range ; j++) {
                        heights[i][j] += dif * (1 - (float) j / range);
                    }
                }
                break;

            case YP:
                for (int i = 0 ; i < size ; i++) {
                    float dif = constraints[k].vertices[i].y - heights[i][size-1];

                    for (int j = 0 ; j < (int) range ; j++) {
                        heights[i][size-1-j] += dif * (1 - (float) j / range);
                    }
                }
                break;

            default:
                break;
        }
    }
	
	float c2[3] = {0.1, 0.1, 0.1};
    float c[3] = {1.0, 1.0, 1.0};

	for (int i = 0 ; i < size ; i++) {
		for (int j = 0 ; j < size ; j++) {
            
   			colors[3*i*size + 3*j] 	   = c[0]*(heights[i][j]/HEIGHT_FACTOR) + c2[0]*(1-heights[i][j]/HEIGHT_FACTOR);
            colors[3*i*size + 3*j + 1] = c[1]*(heights[i][j]/HEIGHT_FACTOR) + c2[1]*(1-heights[i][j]/HEIGHT_FACTOR);
            colors[3*i*size + 3*j + 2] = c[2]*(heights[i][j]/HEIGHT_FACTOR) + c2[2]*(1-heights[i][j]/HEIGHT_FACTOR);

            //coord[2*i*size]

            vertices[3*i*size + 3*j] 	 = i * CHUNK_SIZE / (DEFAULT_MAP_SIZE - 1);
            vertices[3*i*size + 3*j + 1] = heights[i][j];
            vertices[3*i*size + 3*j + 2] = j * CHUNK_SIZE / (DEFAULT_MAP_SIZE - 1);
        }
    }

    for (int i = 0 ; i < size - 1 ; i++) {
    	for (int j = 0 ; j < size ; j++) {
    		indices[2*i*size + 2*j] = i*size + j;
    		indices[2*i*size + 2*j + 1] = i*size + j + size;
    	}
    }

	glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, (size*size*3*sizeof *vertices + size*size*3*sizeof *coord), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 							 (size*size*3*sizeof *vertices), vertices);
    //glBufferSubData(GL_ARRAY_BUFFER, (size*size*3*sizeof *vertices), (size*size*2*sizeof *coord), coord);
    glBufferSubData(GL_ARRAY_BUFFER, (size*size*3*sizeof *vertices), (size*size*3*sizeof *colors), colors);

    glBindBuffer(GL_ARRAY_BUFFER, 0);


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2*size*(size-1) * sizeof *indices, NULL, GL_STATIC_DRAW);    
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, 2*size*(size-1) * sizeof *indices, indices);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Heightmap::draw() const {
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0));
    glColorPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(size*size*3*sizeof *vertices));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    glEnableClientState(GL_VERTEX_ARRAY);
    //glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    for (int i = 0 ; i < size - 1 ; i++) {
    	glDrawElements(GL_TRIANGLE_STRIP, 2*size, GL_UNSIGNED_INT, BUFFER_OFFSET(2*i*size* sizeof *indices));
    }

    glDisableClientState(GL_VERTEX_ARRAY);
    //glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Heightmap::saveToImage() const {
	sf::Uint8* pixels = new sf::Uint8[size * size * 4];

	for (int i = 0 ; i < size ; i++) { // Convert heights to array of pixels
		for (int j = 0 ; j < size ; j++) {
			for (int k = 0 ; k < 3 ; k++) {
				pixels[i*4*size + j*4 + k] = heights[i][j]*255 / HEIGHT_FACTOR;
			}
			pixels[i*4*size + j*4 + 3] = 255;
		}
	}
	
	sf::Texture texture;
	texture.create(size, size);
	texture.update(pixels);

    std::ostringstream convert;
    convert << chunkPos.x << "." << chunkPos.y << ".png"; 

	texture.copyToImage().saveToFile(convert.str());
}

int Heightmap::compareToCorners(sf::Vector3f cam, sf::Vector3f vec) const {
    float dots[4];

    dots[0] = vu::dot(sf::Vector3f(chunkPos.x * CHUNK_SIZE, heights[0][0], chunkPos.y * CHUNK_SIZE)-cam, vec);
    dots[1] = vu::dot(sf::Vector3f(chunkPos.x * CHUNK_SIZE, heights[0][size-1], (chunkPos.y+1) * CHUNK_SIZE)-cam, vec);
    dots[2] = vu::dot(sf::Vector3f((chunkPos.x+1) * CHUNK_SIZE, heights[size-1][size-1], (chunkPos.y+1) * CHUNK_SIZE)-cam, vec);
    dots[3] = vu::dot(sf::Vector3f((chunkPos.x+1) * CHUNK_SIZE, heights[size-1][0], chunkPos.y * CHUNK_SIZE)-cam, vec);
    
    if (dots[0] >= 0.f && dots[1] >= 0.f &&
        dots[2] >= 0.f && dots[3] >= 0.f )
        return 1;

    else if (dots[0] <= 0.f && dots[1] <= 0.f &&
             dots[2] <= 0.f && dots[3] <= 0.f )
        return -1;

    else
        return 0;
}

bool Heightmap::calculateFrustum(const Camera* camera) {
    float theta = camera->getTheta();
    float phi   = camera->getPhi();
    float alpha = camera->getFov() * camera->getRatio() / 2;

    // Bottom of the view
    sf::Vector3f norm = vu::carthesian(1., theta, phi + 90. - camera->getFov() / 2.);
    float tmp;
    tmp = norm.y;
    norm.y = norm.z;
    norm.z = tmp;
    sf::Vector3f pos = camera->getPos();

    if (compareToCorners(pos,norm) == 1)
        return false;

    // Top
    norm = vu::carthesian(1., theta, phi + 90. + camera->getFov() / 2.);
    tmp = norm.y;
    norm.y = norm.z;
    norm.z = tmp;
    norm *= -1.f;

    if (compareToCorners(pos,norm) == 1)
        return false;

    // Right
    norm = vu::carthesian(1., theta + 90.f, 90.f);
    vu::Mat3f rot;
    rot.rotation(vu::carthesian(1., theta + 180., 90.f - phi), - alpha);
    norm = rot.multiply(norm);
    tmp = norm.y;
    norm.y = norm.z;
    norm.z = tmp;

    if (compareToCorners(pos,norm) == 1)
        return false;

    // Left
    norm = vu::carthesian(1., theta - 90.f, 90.f);
    rot.rotation(vu::carthesian(1., theta + 180., 90.f - phi), alpha);
    norm = rot.multiply(norm);
    tmp = norm.y;
    norm.y = norm.z;
    norm.z = tmp;

    if (compareToCorners(pos,norm) == 1)
        return false;

    displayed = true;
    return true;
}

Constraint Heightmap::getConstraint(sf::Vector2i fromChunkPos) const {
    Constraint c;
    c.vertices = std::vector<sf::Vector3f>(size);

    if(fromChunkPos == sf::Vector2i(chunkPos.x+1, chunkPos.y)) {
        for (int i = 0 ; i < size ; i++)
            c.vertices[i] = sf::Vector3f(CHUNK_SIZE, heights[size-1][i], i * CHUNK_SIZE / (size-1));
            
        c.type = XN;
    }

    else if(fromChunkPos == sf::Vector2i(chunkPos.x-1, chunkPos.y)) {
        for (int i = 0 ; i < size ; i++)
            c.vertices[i] = sf::Vector3f(0.f, heights[0][i], i * CHUNK_SIZE / (size-1));

        c.type = XP;
    }

    else if(fromChunkPos == sf::Vector2i(chunkPos.x, chunkPos.y+1)) {
        for (int i = 0 ; i < size ; i++)
            c.vertices[i] = sf::Vector3f(i * CHUNK_SIZE / (size-1), heights[i][size-1], CHUNK_SIZE);

        c.type = YN;
    }

    else if(fromChunkPos == sf::Vector2i(chunkPos.x, chunkPos.y-1)) {
        for (int i = 0 ; i < size ; i++)
            c.vertices[i] = sf::Vector3f( i * CHUNK_SIZE / (size-1), heights[i][0], 0.f);
            
        c.type = YP;
    }

    else
        c.type = NONE;

    return c;
}

float Heightmap::getHeight(float x, float y) const {
    x *= (size - 1) / CHUNK_SIZE;
    y *= (size - 1) / CHUNK_SIZE;

    int iX = x;
    int iY = y;

    float fX = x - iX;
    float fY = y - iY;

    return  heights[iX][iY] + 
            (heights[iX+1][iY] - heights[iX][iY]) * fX + 
            (heights[iX][iY+1] - heights[iX][iY]) * fY + 
            (heights[iX+1][iY+1] + heights[iX][iY] - heights[iX+1][iY] - heights[iX][iY+1]) * fX * fY;
}

