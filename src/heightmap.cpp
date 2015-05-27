#include "heightmap.h"
#include "perlin.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>

#define DEFAULT_MAP_SIZE 257 // +1 to join with other chunks
#define BUFFER_OFFSET(a) ((char*)NULL + (a))


Heightmap::Heightmap(sf::Vector2i chunkPosition, int _seed) :
	size(DEFAULT_MAP_SIZE),
	Chunk(chunkPosition),
    seed(_seed) {
	for (unsigned int i = 0 ; i < DEFAULT_MAP_SIZE ; i++) {
		heights.push_back(std::vector<float>(DEFAULT_MAP_SIZE, 0.0f));
	}

	vertices = new float[size*size*3];
	colors = new float[size*size*3];
	indices = new GLuint[2*size*(size-1)];
}

void Heightmap::generate() {
    Perlin perlin(seed, size);

	for (int i = 0 ; i < size ; i++) {
		for (int j = 0 ; j < size ; j++) {
			heights[i][j] = perlin.getValue(i, j) * HEIGHT_FACTOR;
		}
	}
	
	float c2[3] = {0.1, 0.1, 0.1};
    float c[3] = {1.0, 1.0, 1.0};

	for (int i = 0 ; i < size ; i++) {
		for (int j = 0 ; j < size ; j++) {
            
   			colors[3*i*size + 3*j] 	   = c[0]*(heights[i][j]/HEIGHT_FACTOR) + c2[0]*(1-heights[i][j]/HEIGHT_FACTOR);
            colors[3*i*size + 3*j + 1] = c[1]*(heights[i][j]/HEIGHT_FACTOR) + c2[1]*(1-heights[i][j]/HEIGHT_FACTOR);
            colors[3*i*size + 3*j + 2] = c[2]*(heights[i][j]/HEIGHT_FACTOR) + c2[2]*(1-heights[i][j]/HEIGHT_FACTOR);

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

    glBufferData(GL_ARRAY_BUFFER, (size*size*3*sizeof *vertices + size*size*3*sizeof *colors), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 							 (size*size*3*sizeof *vertices), vertices);
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
    glEnableClientState(GL_COLOR_ARRAY);

    for (int i = 0 ; i < size - 1 ; i++) {
    	glDrawElements(GL_TRIANGLE_STRIP, 2*size, GL_UNSIGNED_INT, BUFFER_OFFSET(2*i*size* sizeof *indices));
    }

    glDisableClientState(GL_VERTEX_ARRAY);
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

bool Heightmap::calculateFrustum(const Camera* camera) {
    return true;
}

float Heightmap::getHeight(float x, float y) const {
    int iX = x * (size - 1) / CHUNK_SIZE;
    int iY = y * (size - 1) / CHUNK_SIZE;

    float fX = x - iX;
    float fY = y - iY;

    return  heights[iX][iY] + 
            (heights[iX+1][iY] - heights[iX][iY]) * fX + 
            (heights[iX][iY+1] - heights[iX][iY]) * fY + 
            (heights[iX+1][iY+1] + heights[iX][iY] - heights[iX+1][iY] - heights[iX][iY+1]) * fX * fY;
}

