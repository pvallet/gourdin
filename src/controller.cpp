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
  map("res/map/"),
  running(true),
	window(_window),
  game(&camera, &map) {

  font.loadFromFile("res/Arial.ttf");
  fpsCounter.setFont(font);
  fpsCounter.setCharacterSize(10);
  fpsCounter.setColor(sf::Color::White);

  rectSelect.setFillColor(sf::Color::Transparent);
  rectSelect.setOutlineThickness(1);
  rectSelect.setOutlineColor(sf::Color(255, 255, 255));

  minimap.setTexture(*(map.getMinimap()));
  minimap.setPosition(sf::Vector2f(0.f, window->getSize().y - minimap.getTextureRect().height));
}

void Controller::init() {
	camera.resize(window->getSize().x, window->getSize().y );
}

void Controller::render() {
  if (running) {

    // Game

    window->clear(sf::Color::Black);
    game.render();
    window->pushGLStates();

    // Life bars

    sf::RectangleShape lifeBar(sf::Vector2f(20., 2.));
    lifeBar.setFillColor(sf::Color::Green);
    lifeBar.setOutlineThickness(0);
    sf::RectangleShape fullLifeBar(sf::Vector2f(20., 2.));
    fullLifeBar.setFillColor(sf::Color::Transparent);
    fullLifeBar.setOutlineColor(sf::Color::Black);
    fullLifeBar.setOutlineThickness(1);

    std::set<igElement*> sel = game.getSelection();
    sf::IntRect corners;
    float maxHeightFactor;

    for(auto it = sel.begin(); it != sel.end(); ++it) {
      corners = (*it)->get2DCorners();
      Controllable* ctrl = (Controllable*) (*it);
      maxHeightFactor = ctrl->getMaxHeightFactor(); // The lifeBar must not change when switching animations

      if (ctrl->getMovingType() == HUNTER) {
        Lion* lion = (Lion*) ctrl;
        lifeBar.setSize(sf::Vector2f(20.* lion->getStamina() / 100., 2.));
      }

      lifeBar.setPosition(corners.left + corners.width/2 - 10, corners.top - corners.height*maxHeightFactor + corners.height - 5);
      fullLifeBar.setPosition(corners.left + corners.width/2 - 10, corners.top - corners.height*maxHeightFactor + corners.height - 5);
      window->draw(lifeBar);
      window->draw(fullLifeBar);
      lifeBar.setSize(sf::Vector2f(20., 2.));
    }

    // RectSelect

    window->draw(rectSelect);

    // Minimap
    window->draw(minimap);

    sf::CircleShape miniCamPos(3);
    miniCamPos.setPointCount(8);
    miniCamPos.setFillColor(sf::Color::Black);
    miniCamPos.setPosition( minimap.getPosition().x - miniCamPos.getRadius() / 2 +
                            (float) minimap.getTextureRect().height * camera.getPointedPos().x / map.getMaxCoord(),

                            minimap.getPosition().y - miniCamPos.getRadius() / 2 +
                            (float) minimap.getTextureRect().width  * camera.getPointedPos().y / map.getMaxCoord());

    window->draw(miniCamPos);

    // FPS

    int fps = 1. / elapsed.asSeconds();
    std::ostringstream convert;
    convert << fps;
    fpsCounter.setString("FPS: " + convert.str());

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
        if (minimap.getTextureRect().contains(sf::Vector2i(event.mouseButton.x, window->getSize().y - event.mouseButton.y))) {
          game.moveCamera(sf::Vector2f( (float) (event.mouseButton.x - minimap.getPosition().x) /
                                        (float) minimap.getTextureRect().width * map.getMaxCoord(),
                                        (float) (event.mouseButton.y - minimap.getPosition().y) /
                                        (float) minimap.getTextureRect().height * map.getMaxCoord()));
        }

        else {
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
            if (game.getSelection().empty())
              game.addLion(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
            else
              game.moveSelection(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
          }
        }
      }

      else if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Left) {
          game.select(sf::IntRect( rectSelect.getPosition().x, rectSelect.getPosition().y,
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
              std::set<igElement*> sel = game.getSelection();

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
              std::set<igElement*> sel = game.getSelection();
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

    if ((int) sf::Mouse::getPosition().x == (int) camera.getW() - 1)
      camera.translate(- TRANSLATION_VALUE_PS * elapsed.asSeconds(), 0.);

    if ((int) sf::Mouse::getPosition().y == (int) camera.getH() - 1)
      camera.translate(0., TRANSLATION_VALUE_PS * elapsed.asSeconds());

    camera.apply();
    game.update(elapsed);
    render();
  }
}
