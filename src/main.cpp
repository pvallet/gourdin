#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <iostream>
#include <thread>
//#include <X11/Xlib.h>

#include "controller.h"

#define DEFAULT_SCREENWIDTH 800
#define DEFAULT_SCREENHEIGHT 600


int main()
{
    //XInitThreads();

    sf::ContextSettings context(32, 8, 4, 3, 0);

    sf::RenderWindow window(sf::VideoMode::getFullscreenModes().front(), "OpenGL", sf::Style::Fullscreen, context);

    glewInit();

    window.setVerticalSyncEnabled(true);
    window.setActive(true);

    glEnable(GL_DEPTH_TEST);

    Controller controller(&window);

    controller.init();

    controller.run();

    return 0;
}