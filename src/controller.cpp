#include "controller.h"
#include <set>
#include <string>
#include <sstream>
#include "igElement.h"
#include "lion.h"

#define ROTATION_ANGLE_PS 60. // PS = per second
#define TRANSLATION_VALUE_PS 60.
#define ZOOM_FACTOR 10.

Controller::Controller(sf::RenderWindow* _window) :
 	addSelect(false),
    selecting(false),
    rectSelect(sf::Vector2f(0., 0.)),
    running(true),
	window(_window),
    map(&camera) {

    font.loadFromFile("res/Arial.ttf");
    fpsCounter.setFont(font);
    fpsCounter.setCharacterSize(10);
    fpsCounter.setColor(sf::Color::White);

    rectSelect.setFillColor(sf::Color::Transparent);
    rectSelect.setOutlineThickness(1);
    rectSelect.setOutlineColor(sf::Color(255, 255, 255));
}

void Controller::init() {
	camera.resize(window->getSize().x, window->getSize().y );
}

void Controller::render() {
    if (running)
    {
        window->clear(sf::Color::Black);
        map.render();
        window->pushGLStates();

        // Life bars

        sf::RectangleShape lifeBar(sf::Vector2f(20., 2.));
        lifeBar.setFillColor(sf::Color::Yellow);
        lifeBar.setOutlineThickness(0);

        std::set<igElement*> sel = map.getSelection();
        sf::IntRect corners;
        float maxHeightFactor;

        for(auto it = sel.begin(); it != sel.end(); ++it) {
            corners = (*it)->get2DCorners();
            maxHeightFactor = (*it)->getMaxHeightFactor(); // The lifeBar must not change when switching animations
            Controllable* ctrl = (Controllable*) (*it);
            
            if (ctrl->getMovingType() == HUNTER) {
                Lion* lion = (Lion*) ctrl;
                lifeBar.setSize(sf::Vector2f(20.* lion->getStamina() / 100., 2.));
            }

            lifeBar.setPosition(corners.left + corners.width/2 - 10, corners.top - corners.height*maxHeightFactor + corners.height - 5);
            window->draw(lifeBar);
            lifeBar.setSize(sf::Vector2f(20., 2.));
        }

        // FPS

        int fps = 1. / elapsed.asSeconds();
        std::ostringstream convert;
        convert << fps; 
        fpsCounter.setString("FPS: " + convert.str());

        window->draw(rectSelect);
        window->draw(fpsCounter);
        window->display();
        window->popGLStates();
    }
}

void Controller::run() {
    while (running) {
        sf::Event event;
        elapsed = frameClock.restart();

        while (window->pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                running = false;

            else if (event.type == sf::Event::Resized) {
                glViewport(0, 0, event.size.width, event.size.height);
                camera.resize(event.size.width, event.size.height);
            }

            else if (event.type == sf::Event::MouseWheelMoved)
                camera.zoom(- ZOOM_FACTOR * event.mouseWheel.delta);

            else if (event.type == sf::Event::MouseButtonPressed) {
                // Begin selection
                if (event.mouseButton.button == sf::Mouse::Left) {
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                        addSelect = true;
                    else
                        addSelect = false;

                    selecting = true;
                    rectSelect.setPosition(event.mouseButton.x, event.mouseButton.y);
                }

                // Move selection
                if (event.mouseButton.button == sf::Mouse::Right) {
                    if (map.getSelection().empty())
                        map.addLion(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                    else
                        map.moveSelection(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                }
            }

            else if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    map.select(sf::IntRect( rectSelect.getPosition().x, rectSelect.getPosition().y,
                                            rectSelect.getSize().x, rectSelect.getSize().y), addSelect);
                    selecting = false;
                    rectSelect.setSize(sf::Vector2f(0,0));
                }
            }

            else if (event.type == sf::Event::MouseMoved) {
                if (selecting) {
                    rectSelect.setSize(sf::Vector2f(event.mouseMove.x - rectSelect.getPosition().x,
                                                    event.mouseMove.y - rectSelect.getPosition().y));
                }
            }

            else if (event.type == sf::Event::KeyPressed) {
                switch(event.key.code) {
                    case sf::Keyboard::Escape:
                        running = false;
                        break;
                    case sf::Keyboard::LShift: {
                        std::set<igElement*> sel = map.getSelection();

                        for (auto it = sel.begin() ; it != sel.end() ; ++it) {
                            Controllable* ctrl = (Controllable*) (*it);

                            if (ctrl->getMovingType() == HUNTER) {
                                Lion* lion = (Lion*) ctrl;
                                if (lion->isRunning())
                                    lion->beginWalking();
                                else
                                    lion->beginRunning();
                            }
                        }}
                        break;
                    default:
                        break;
                }
            }

            else if (event.type == sf::Event::KeyReleased) {
                switch(event.key.code) {
                    case sf::Keyboard::Delete: {
                        std::set<igElement*> sel = map.getSelection();
                        Controllable* ctrl = (Controllable*) (*sel.begin());
                        ctrl->die();
                    }
                    break;

                    default:
                        break;
                }
            }

        }

        // Move camera

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            camera.rotate(- ROTATION_ANGLE_PS * elapsed.asSeconds(), 0.);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            camera.rotate(ROTATION_ANGLE_PS * elapsed.asSeconds(), 0.);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            camera.translate(0., - TRANSLATION_VALUE_PS * elapsed.asSeconds());
            //camera.rotate(0., - ROTATION_ANGLE_PS * elapsed.asSeconds()); // for the mouse wheel

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            camera.translate(0., TRANSLATION_VALUE_PS * elapsed.asSeconds());
            //camera.rotate(0., ROTATION_ANGLE_PS * elapsed.asSeconds());

        if (sf::Mouse::getPosition().x == 0)
            camera.translate(TRANSLATION_VALUE_PS * elapsed.asSeconds(), 0.);

        if (sf::Mouse::getPosition().y == 0)
            camera.translate(0., - TRANSLATION_VALUE_PS * elapsed.asSeconds());

        if (sf::Mouse::getPosition().x == camera.getW() - 1)
            camera.translate(- TRANSLATION_VALUE_PS * elapsed.asSeconds(), 0.);

        if (sf::Mouse::getPosition().y == camera.getH() - 1)
            camera.translate(0., TRANSLATION_VALUE_PS * elapsed.asSeconds());

        camera.apply();
        map.update(elapsed);
        render();
    }
}

