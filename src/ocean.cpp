#include "ocean.h"
#include "vecUtils.h"

#define BUFFER_OFFSET(a) ((char*)NULL + (a))

Ocean::Ocean(sf::Vector2i chunkPosition, GLuint _tex) : 
	Chunk(chunkPosition),
	tex(_tex),
	vertices{0, 0, 0,
			 0, 0, CHUNK_SIZE,
			 CHUNK_SIZE, 0, 0,
			 CHUNK_SIZE, 0, CHUNK_SIZE},

	coord {0, 0,  0, TEX_FACTOR,  TEX_FACTOR, 0,  TEX_FACTOR, TEX_FACTOR},
	normals {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0},
	indices {0, 1, 2, 3} {

    // vbo

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(	GL_ARRAY_BUFFER, (32*sizeof *vertices), NULL, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, (12*sizeof *vertices), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, (12*sizeof *vertices), (12*sizeof *normals), normals);
    glBufferSubData(GL_ARRAY_BUFFER, (24*sizeof *vertices), (8*sizeof *coord), coord);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // ibo

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof *indices, NULL, GL_STATIC_DRAW);    
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, 4 * sizeof *indices, indices);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

int Ocean::compareToCorners(sf::Vector3f cam, sf::Vector3f vec) const {
    float dots[4];

    dots[0] = vu::dot(sf::Vector3f(chunkPos.x * CHUNK_SIZE, 0, chunkPos.y * CHUNK_SIZE)-cam, vec);
    dots[1] = vu::dot(sf::Vector3f(chunkPos.x * CHUNK_SIZE, 0, (chunkPos.y+1) * CHUNK_SIZE)-cam, vec);
    dots[2] = vu::dot(sf::Vector3f((chunkPos.x+1) * CHUNK_SIZE, 0, (chunkPos.y+1) * CHUNK_SIZE)-cam, vec);
    dots[3] = vu::dot(sf::Vector3f((chunkPos.x+1) * CHUNK_SIZE, 0, chunkPos.y * CHUNK_SIZE)-cam, vec);
    
    if (dots[0] >= 0.f && dots[1] >= 0.f &&
        dots[2] >= 0.f && dots[3] >= 0.f )
        return 1;

    else if (dots[0] <= 0.f && dots[1] <= 0.f &&
             dots[2] <= 0.f && dots[3] <= 0.f )
        return -1;

    else
        return 0;
}

void Ocean::calculateFrustum(const Camera* camera) {
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

    if (compareToCorners(pos,norm) == 1) {
        visible = false;
        return;
    }

    // Top
    norm = vu::carthesian(1., theta, phi + 90. + camera->getFov() / 2.);
    tmp = norm.y;
    norm.y = norm.z;
    norm.z = tmp;
    norm *= -1.f;

    if (compareToCorners(pos,norm) == 1) {
        visible = false;
        return;
    }

    // Right
    norm = vu::carthesian(1., theta + 90.f, 90.f);
    vu::Mat3f rot;
    rot.rotation(vu::carthesian(1., theta + 180., 90.f - phi), - alpha);
    norm = rot.multiply(norm);
    tmp = norm.y;
    norm.y = norm.z;
    norm.z = tmp;

    if (compareToCorners(pos,norm) == 1) {
        visible = false;
        return;
    }

    // Left
    norm = vu::carthesian(1., theta - 90.f, 90.f);
    rot.rotation(vu::carthesian(1., theta + 180., 90.f - phi), alpha);
    norm = rot.multiply(norm);
    tmp = norm.y;
    norm.y = norm.z;
    norm.z = tmp;

    if (compareToCorners(pos,norm) == 1) {
        visible = false;
        return;
    }

    visible = true;
}
	
void Ocean::draw() const {
    glBindTexture(GL_TEXTURE_2D, tex);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0));
    glNormalPointer(GL_FLOAT, 0, BUFFER_OFFSET(12*sizeof *vertices));
    glTexCoordPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(24*sizeof *vertices));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, BUFFER_OFFSET(0));

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindTexture(GL_TEXTURE_2D, 0);
}
