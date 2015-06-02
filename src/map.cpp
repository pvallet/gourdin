#include "map.h"
#include <SFML/OpenGL.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>

#define MIN_ANTILOPE_PROX 2.f
#define HERD_RADIUS 5.f // for 10 antilopes

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

    hmap[0]->generate(std::vector<Constraint>());
    map.insert(std::pair<sf::Vector2i, Chunk*>(hmap[0]->getChunkPos(), hmap[0]));

    for (int i = 1 ; i < 6 ; i++) {
        std::vector<Constraint> c;

        for (int j = 0 ; j < i ; j++)
            c.push_back(hmap[j]->getConstraint(hmap[i]->getChunkPos()));

        hmap[i]->generate(c);
        map.insert(std::pair<sf::Vector2i, Chunk*>(hmap[i]->getChunkPos(), hmap[i]));
    }

    skybox = new Skybox("res/skybox/skybox", cam);

    sf::Color mask(0, 151, 135);

    std::vector<std::string> lionSheets;
    lionSheets.push_back("res/lion/wait.png");
    lionSheets.push_back("res/lion/walk.png");
    lionSheets.push_back("res/lion/die.png");
    lionSheets.push_back("res/lion/run.png");
    lionSheets.push_back("res/lion/attack.png");

    for (unsigned int i = 0 ; i < lionSheets.size() ; i++) {
        sf::Image img;

        if (!img.loadFromFile(lionSheets[i])) {
            std::cout << "Unable to open file" << std::endl;
        }

        sf::Texture* curTex = new sf::Texture();

        img.createMaskFromColor(mask);

        curTex->loadFromImage(img);
        curTex->setSmooth(true);

        lionTex.push_back(curTex);
    }

    std::vector<std::string> antilopeSheets;
    antilopeSheets.push_back("res/antilope/wait.png");
    antilopeSheets.push_back("res/antilope/walk.png");
    antilopeSheets.push_back("res/antilope/die.png");
    antilopeSheets.push_back("res/antilope/run.png");

    for (unsigned int i = 0 ; i < antilopeSheets.size() ; i++) {
        sf::Image img;

        if (!img.loadFromFile(antilopeSheets[i])) {
            std::cerr << "Unable to open file" << std::endl;
        }

        sf::Texture* curTex = new sf::Texture();

        img.createMaskFromColor(mask);

        curTex->loadFromImage(img);
        curTex->setSmooth(true);

        antilopeTex.push_back(curTex);
    }

    generateHerd(sf::Vector2f(0.f,0.f), 20);
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
    for (unsigned int i = 0 ; i < e.size() ; i++) {
        if (e[i]->getAbstractType() != igE) {
            igMovingElement* igM = (igMovingElement*) e[i];
            if (igM->getMovingType() == PREY) {
                Antilope* atlp = (Antilope*) igM;
                atlp->updateState(e);
            }

            else if (igM->getMovingType() == HUNTER) {
                Lion* lion = (Lion*) igM;
                lion->kill(e);
            }
        }
    }

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

    sortE();
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

void Map::sortE() {
    
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
                if (e[i]->getAbstractType() == CTRL)
                    sel.insert(e[i]);
            }

            else if (   c.contains(rect.left, rect.top) ||
                        c.contains(rect.left + rect.width, rect.top) ||
                        c.contains(rect.left + rect.width, rect.top + rect.height) ||
                        c.contains(rect.left, rect.top + rect.height)  ) {
                if (e[i]->getAbstractType() == CTRL)
                    sel.insert(e[i]);   
            }
        }
    }
}

void Map::moveSelection(sf::Vector2i screenTarget) {
    sf::Vector2f target = get2DCoord(screenTarget);

    for(auto it = sel.begin(); it != sel.end(); ++it) {
        if ((*it)->getAbstractType() == CTRL) {
            Controllable* tmp = (Controllable*) *it;
            tmp->setTarget(target);
        }
    }
}

void Map::addLion(sf::Vector2i screenTarget) {
    e.push_back(new Lion(get2DCoord(screenTarget), AnimationManager(lionTex, "res/lion/animInfo.xml")));
}

void Map::generateHerd(sf::Vector2f pos, int count) {
    srand(time(NULL));
    float r, theta;
    sf::Vector2f p, diff;
    bool add;

    std::vector<Antilope*> tmp;

    for (int i = 0 ; i < count ; i++) {
        add = true;
        r = sqrt((float) rand() / (float) RAND_MAX) * HERD_RADIUS * sqrt(count);
        theta = (float) rand() / (float) RAND_MAX * 2*M_PI;

        p.x = pos.x + r*cos(theta);
        p.y = pos.y + r*sin(theta);

        for (int j = 0 ; j < i ; j++) {
            diff = tmp[j]->getPos() - p;
            
            if (diff.x * diff.x + diff.y * diff.y < MIN_ANTILOPE_PROX) {
                i--;
                add = false;
            }
        }

        if (add) {
            tmp.push_back(new Antilope(p, AnimationManager(antilopeTex, "res/antilope/animInfo.xml")));
        }
    }

    for (int i = 0 ; i < count ; i++) {
        e.push_back(tmp[i]);
    }
}

sf::Vector2f Map::get2DCoord(sf::Vector2i screenTarget) const {
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

    return sf::Vector2f(x,z);
}


