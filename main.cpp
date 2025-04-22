#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <sstream>


#include "UI-UX/Background.h"
#include "UI-UX/Map.h"
#include "UI-UX/NaviGator.h"
#include "UI-UX/Title.h"
#include "UI-UX/Soundtrack.h"
#include "UI-UX/UX.h"
#include "UI-UX/GPS.h"
#include "algorithms/Algorithms.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "The Floor is Lava!!!");
    window.setFramerateLimit(60);

    Soundtrack soundtrack;
    soundtrack.loadFromFile("assets/soundtrack.mp3");
    soundtrack.play();

    Background background("assets/sky.jpeg");
    Title title;
    while (window.isOpen() && !title.isFinished()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            title.handleEvent(event);
        }
        window.clear();
        window.setView(window.getDefaultView());
        background.draw(window);
        title.draw(window);
        window.display();
    }

    Map gameMap;
    if (!gameMap.loadFromFile("maps/floorIsLava.tmx", 32, 16)) return 1;

    sf::View view;
    view.setSize(400, 300);
    float centerX = ((gameMap.getWidth() - gameMap.getHeight()) * 16.f) / 2.f;
    float centerY = ((gameMap.getWidth() + gameMap.getHeight()) * 8.f) / 2.f;
    view.setCenter(centerX, centerY);
    window.setView(view);

    Background gameBackground("assets/sky.jpeg");
    NaviGator navigator("sprites/navigator.png", sf::Vector2f(0.f, 0.f));
    sf::Clock clock;
    UX ux("The Floor is Lava!\nUse WASD to move.");
    ux.setSecondaryMessage("Click the box below and type a destination tile (e.g. 65 9)");
    ux.setInstructionMessage("Then press Enter to confirm.");

    sf::Vector2i enteredPointB;
    GPS gps;
    bool coordinatesReady = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            ux.handleEvent(event, window);

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter && ux.hasValidInput()) {
                enteredPointB = ux.getTypedCoordinates();
                coordinatesReady = true;

                std::stringstream ss;
                ss << "Destination set to (" << enteredPointB.x << ", " << enteredPointB.y << ")\n";
                ss << "Press 1 for Dijkstra, 2 for A*, 3 to compare both";
                ux.setConfirmationMessage(ss.str());
                ux.resetTypedCoordinates();
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                coordinatesReady = false;
                ux.resetTypedCoordinates();
                ux.setInstructionMessage("Re-enter coordinates and press Enter");
                ux.setSecondaryMessage("Click the box below and type a destination");
                ux.clearConfirmationMessage();
                gps.clearPath();
            }

            if (event.type == sf::Event::MouseWheelScrolled) {
                if (event.mouseWheelScroll.delta > 0)
                    view.zoom(0.9f);
                else
                    view.zoom(1.1f);
                window.setView(view);
            }


            if (coordinatesReady && event.type == sf::Event::KeyPressed) {
                float ax = (navigator.getPosition().y / 8.0f + navigator.getPosition().x / 16.0f) / 2.0f;
                float ay = (navigator.getPosition().y / 8.0f - navigator.getPosition().x / 16.0f) / 2.0f;
                sf::Vector2i pointA(static_cast<int>(ax), static_cast<int>(ay));

                if (event.key.code == sf::Keyboard::Num1) {
                    gps.setPath(findDijkstraPath(gameMap, pointA, enteredPointB), DIJKSTRA);

                    ux.clearConfirmationMessage();
                } else if (event.key.code == sf::Keyboard::Num2) {
                    gps.setPath(findAStarPath(gameMap, pointA, enteredPointB), ASTAR);

                    ux.clearConfirmationMessage();
                } else if (event.key.code == sf::Keyboard::Num3) {
                    gps.setComparisonPaths(findDijkstraPath(gameMap, pointA, enteredPointB),
                                           findAStarPath(gameMap, pointA, enteredPointB));

                    ux.clearConfirmationMessage();
                }
            }

            navigator.handleInput(sf::Time::Zero, event, view);
        }

        sf::Time frameTime = clock.restart();
        navigator.handleInput(frameTime, sf::Event(), view);
        navigator.update(frameTime);
        gameMap.updateLavaSpread(frameTime.asSeconds());

        gps.update(frameTime.asSeconds());
        view.setCenter(navigator.getPosition());
        window.setView(view);

        window.clear(sf::Color::Black);
        window.setView(window.getDefaultView());
        gameBackground.draw(window);

        window.setView(view);
        gameMap.draw(window);
        gps.draw(window);
        navigator.draw(window);

        window.setView(window.getDefaultView());
        ux.updateCursor();
        ux.draw(window);
        window.display();
    }

    return 0;
}


/*
 * MUSIC:
* Music from #Uppbeat (free for Creators!):
https://uppbeat.io/t/color-parade/pixel-playground
License code: YXK8LCR0AWJD0S3M

CITATIONS:
https://www.sfml-dev.org/documentation/3.0.0/
https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-greedy-algo-7/
https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-greedy-algo-7/
https://github.com/leethomason/tinyxml2.git
*/