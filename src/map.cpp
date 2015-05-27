#include "map.h"
#include <SFML/OpenGL.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>

Map::Map(Camera* camera) :
    cam(camera) {
	
    Heightmap* hmap[6];
    
    srand(time(NULL));

    hmap[0] = new Heightmap(sf::Vector2i( 0, 0), rand());
    hmap[1] = new Heightmap(sf::Vector2i( 0,-1), rand());
    hmap[2] = new Heightmap(sf::Vector2i(-1, 0), rand());
    hmap[3] = new Heightmap(sf::Vector2i(-1,-1), rand());
    hmap[4] = new Heightmap(sf::Vector2i(-2, 0), rand());
    hmap[5] = new Heightmap(sf::Vector2i(-2,-1), rand());

    hmap[0]->generate();
    hmap[0]->saveToImage();
    map.insert(std::pair<sf::Vector2i, Chunk*>(hmap[0]->getChunkPos(), hmap[0]));

    for (int i = 1 ; i < 6 ; i++) {
        hmap[i]->generate();
        hmap[i]->saveToImage();
        map.insert(std::pair<sf::Vector2i, Chunk*>(hmap[i]->getChunkPos(), hmap[i]));
    }

    e.push_back(new igMovingElement(sf::Vector2f(0.,0.), "res/caveman.png"));

    skybox = new Skybox("res/skybox", cam);
}

Map::~Map() {
	for(auto it = map.begin() ; it != map.end() ; ++it) {
        //delete it->second;
    }

    delete skybox;
    for (unsigned int i = 0 ; i < e.size() ; i++) {
        delete e[i];
    }
}

void Map::update(sf::Time elapsed) {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,viewport);
    GLdouble projection[16];
    glGetDoublev(GL_PROJECTION_MATRIX,projection);
    GLdouble modelview[16];
    glGetDoublev(GL_MODELVIEW_MATRIX,modelview);

    for (unsigned int i = 0 ; i < e.size() ; i++) {
        e[i]->update(elapsed, cam->getTheta()); // Choose the right sprite and update pos

        int chunkPosX = e[i]->getPos().x < 0 ? e[i]->getPos().x / CHUNK_SIZE - 1 : e[i]->getPos().x / CHUNK_SIZE;
        int chunkPosY = e[i]->getPos().y < 0 ? e[i]->getPos().y / CHUNK_SIZE - 1 : e[i]->getPos().y / CHUNK_SIZE;

        // No test yet to see if the element can move to its new pos (no collision)
        float newHeight =   map[sf::Vector2i(chunkPosX, chunkPosY)]
                            ->getHeight(e[i]->getPos().x - CHUNK_SIZE * chunkPosX,
                                        e[i]->getPos().y - CHUNK_SIZE * chunkPosY);

        // Calculate new corners
        sf::Vector3f corners3[4];

        float width = e[i]->getH() * e[i]->getCurrentSprite().width / e[i]->getCurrentSprite().height;

        corners3[0] = sf::Vector3f( e[i]->getPos().x + sin(cam->getTheta()*M_PI/180.)*width/2,
                                    newHeight + e[i]->getH(),
                                    e[i]->getPos().y - cos(cam->getTheta()*M_PI/180.)*width/2);
            

        corners3[1] = sf::Vector3f( e[i]->getPos().x - sin(cam->getTheta()*M_PI/180.)*width/2,
                                    newHeight + e[i]->getH(),
                                    e[i]->getPos().y + cos(cam->getTheta()*M_PI/180.)*width/2);
            

        corners3[2] = sf::Vector3f( e[i]->getPos().x - sin(cam->getTheta()*M_PI/180.)*width/2,
                                    newHeight,
                                    e[i]->getPos().y + cos(cam->getTheta()*M_PI/180.)*width/2);
            

        corners3[3] = sf::Vector3f( e[i]->getPos().x + sin(cam->getTheta()*M_PI/180.)*width/2,
                                    newHeight,
                                    e[i]->getPos().y - cos(cam->getTheta()*M_PI/180.)*width/2);

        e[i]->set3DCorners(corners3);

        // Calculate their projections

        GLdouble left, top, right, bot;
        GLdouble trash;

        gluProject(corners3[0].x,corners3[0].y,corners3[0].z,modelview,projection,viewport,&left,  &top,   &trash);
        gluProject(corners3[1].x,corners3[1].y,corners3[1].z,modelview,projection,viewport,&right, &trash, &trash);
        gluProject(corners3[3].x,corners3[3].y,corners3[3].z,modelview,projection,viewport,&trash, &bot,   &trash);

        top = viewport[3]-top;
        bot = viewport[3]-bot;

        sf::IntRect cornersRect((int) left, (int) top, (int) right-left, (int) bot-top);

        e[i]->set2DCorners(cornersRect);

    }
}

void Map::render() const {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Skybox

    glEnable(GL_TEXTURE_CUBE_MAP_ARB);
    glDepthMask(GL_FALSE);
    skybox->draw();
    glDepthMask(GL_TRUE);
    glDisable(GL_TEXTURE_CUBE_MAP_ARB);
    
    // Heightmap

    for(auto it = map.begin() ; it != map.end() ; ++it) {
        if (it->second->calculateFrustum(cam)) {
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();

            glTranslatef(CHUNK_SIZE * it->first.x, 0.0f, CHUNK_SIZE * it->first.y);
        
            it->second->draw();
            
        
            glPopMatrix();
        }
    }

    // igElements

    glEnable (GL_TEXTURE_2D);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (unsigned int i = 0 ; i < e.size() ; i++) {
        const sf::Texture* texture = e[i]->getTexture();
        sf::IntRect rect = e[i]->getCurrentSprite();

        sf::Texture::bind(texture, sf::Texture::CoordinateType::Pixels);

        glBegin( GL_QUADS );
            glTexCoord2d(rect.left, rect.top);
            glVertex3f( e[i]->get3DCorners()[0].x, e[i]->get3DCorners()[0].y, e[i]->get3DCorners()[0].z);
            
            glTexCoord2d(rect.left + rect.width,rect.top); 
            glVertex3f( e[i]->get3DCorners()[1].x, e[i]->get3DCorners()[1].y, e[i]->get3DCorners()[1].z);
            
            glTexCoord2d(rect.left + rect.width,rect.top + rect.height);
            glVertex3f( e[i]->get3DCorners()[2].x, e[i]->get3DCorners()[2].y, e[i]->get3DCorners()[2].z);
            
            glTexCoord2d(rect.left, rect.top + rect.height);
            glVertex3f( e[i]->get3DCorners()[3].x, e[i]->get3DCorners()[3].y, e[i]->get3DCorners()[3].z);
        glEnd();

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glMatrixMode(GL_TEXTURE); // sf::Texture::bind modifies the texture matrix, we need to set it back to identity
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);

    glDisable(GL_BLEND | GL_TEXTURE_2D);
}


void Map::select(sf::IntRect rect, bool add) {    
    if (!add)
        sel.clear();

    for (unsigned int i = 0 ; i < e.size() ; i++) {
        if (sel.find(e[i]) == sel.end()) { // e[i] is not selected yet, we can bother to calculate
            sf::IntRect c = e[i]->get2DCorners();

            int centerX, centerY;

            centerX = c.left + c.width / 2;
            centerY = c.top + c.height / 2;

            if (rect.contains(centerX, centerY)) {
                sel.insert(e[i]);
            }

            else if (   c.contains(rect.left, rect.top) ||
                        c.contains(rect.left + rect.width, rect.top) ||
                        c.contains(rect.left + rect.width, rect.top + rect.height) ||
                        c.contains(rect.left, rect.top + rect.height)  ) {
                sel.insert(e[i]);   
            }
        }
    }
}

void Map::moveSelection(sf::Vector2i screenTarget) {

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,viewport);
    GLdouble projection[16];
    glGetDoublev(GL_PROJECTION_MATRIX,projection);
    GLdouble modelview[16];
    glGetDoublev(GL_MODELVIEW_MATRIX,modelview);
    GLdouble winX,winY;
    
    screenTarget.y=viewport[3]-screenTarget.y; // Inverted coordinates
    
    winX=(double)screenTarget.x;
    winY=(double)screenTarget.y;

    GLfloat d;
    glReadPixels(screenTarget.x, screenTarget.y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &d);

    double x,y,z;
    gluUnProject(winX,winY,d,modelview,projection,viewport,&x,&y,&z);

    if (z != 1.) { // If the click was on the map
        for(auto it = sel.begin(); it != sel.end(); ++it) {
            if ((*it)->getType() == igME) {
                igMovingElement* tmp = (igMovingElement*) *it;
                tmp->setTarget(sf::Vector2f(x,z));
            }
        }
    }
}




