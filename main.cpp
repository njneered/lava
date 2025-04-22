#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <random>


#include "UI-UX/Background.h"
#include "UI-UX/Map.h"
#include "UI-UX/NaviGator.h"
#include "UI-UX/Title.h"
#include "UI-UX/Soundtrack.h"
#include "UI-UX/UX.h"
#include "UI-UX/GPS.h"
#include "algorithms/Algorithms.h"

bool verifyPathSegments(const std::vector<sf::Vector2i>& path, const Map& map) {
    for (size_t i = 0; i + 1 < path.size(); ++i) {
        sf::Vector2i a = path[i], b = path[i + 1];
        int dx = abs(a.x - b.x), dy = abs(a.y - b.y);
        if (dx > 1 || dy > 1) return false;
        if (!map.isWalkable(a.x, a.y) || !map.isWalkable(b.x, b.y)) return false;
    }
    return true;
}

int main() {

    sf::RenderWindow window(sf::VideoMode(1280, 720), "The Floor is Lava!!!");
    window.setFramerateLimit(60);

    // Test texture loading
    sf::Texture testTexture;
    if (testTexture.loadFromFile("sprites/spritesheet.png")) {
        std::cout << "Successfully loaded test texture: "
                << testTexture.getSize().x << "x" << testTexture.getSize().y << std::endl;
    } else {
        std::cerr << "Failed to load test texture!" << std::endl;
    }

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

    // Load the map
    Map gameMap;
    if (!gameMap.loadFromFile("maps/floorIsLava.tmx", 32, 16)) {
        std::cerr << "Failed to load map file. Using an empty map." << std::endl;
    }

    // Generate random seed for lava
    std::random_device rd;
    std::mt19937 rng(rd());
    unsigned int seed = rng();

    // Initialize and generate lava
    gameMap.initLavaGenerator(seed);

    // Generate the lava
    gameMap.generateLava();

    // Set up the view
    sf::View view;
    view.setSize(400, 300);
    float centerX = ((gameMap.getWidth() - gameMap.getHeight()) * 16.f) / 2.f;
    float centerY = ((gameMap.getWidth() + gameMap.getHeight()) * 8.f) / 2.f;
    view.setCenter(centerX, centerY);
    window.setView(view);

    // Set up the game UI and objects
    Background gameBackground("assets/sky.jpeg");
    NaviGator navigator("sprites/navigator.png", sf::Vector2f(0.f, 0.f));
    sf::Clock clock;
    UX ux("The Floor is Lava!\nUse WASD to move.");
    ux.setSecondaryMessage("Click the box below and type a destination tile (e.g. 65 9)");
    ux.setInstructionMessage("Then press Enter to confirm.");

    sf::Vector2i enteredPointB;
    GPS gps;
    bool coordinatesReady = false;

    // Main game loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            ux.handleEvent(event, window);

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter && ux.hasValidInput()) {
                enteredPointB = ux.getTypedCoordinates();
                coordinatesReady = true;

                // Check if destination is on lava
                if (gameMap.isLava(enteredPointB.x, enteredPointB.y)) {
                    ux.setConfirmationMessage("That destination is on lava! Try a different location.");
                    coordinatesReady = false;
                } else {
                    std::stringstream ss;
                    ss << "Destination set to (" << enteredPointB.x << ", " << enteredPointB.y << ")\n";
                    ss << "Press 1 for Dijkstra, 2 for A*, 3 to compare both";
                    ux.setConfirmationMessage(ss.str());
                }

                ux.resetTypedCoordinates();
            }

            // Reset button
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                coordinatesReady = false;
                ux.resetTypedCoordinates();
                ux.setInstructionMessage("Re-enter coordinates and press Enter");
                ux.setSecondaryMessage("Click the box below and type a destination");
                ux.clearConfirmationMessage();
                gps.clearPath();
            }

            // Regenerate lava with G key
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::G) {
                // New random seed
                seed = rng();
                gameMap.initLavaGenerator(seed);
                gameMap.generateLava();
                ux.setConfirmationMessage("Lava regenerated with new seed: " + std::to_string(seed));
            }

            // Zoom control
            if (event.type == sf::Event::MouseWheelScrolled) {
                if (event.mouseWheelScroll.delta > 0)
                    view.zoom(0.9f);
                else
                    view.zoom(1.1f);
                window.setView(view);
            }

            // Pathfinding key handling
            if (coordinatesReady && event.type == sf::Event::KeyPressed) {
                // Convert navigator position to tile coordinates
                sf::Vector2f navigatorPos = navigator.getPosition();
                sf::Vector2i pointA = gameMap.screenToTile(navigatorPos.x, navigatorPos.y);

                // Check if we're standing on lava
                if (gameMap.isLava(pointA.x, pointA.y)) {
                    ux.setConfirmationMessage("You're standing on lava! Move to a safe location first.");
                    continue;
                }

                if (event.key.code == sf::Keyboard::Num1) {
                    auto path = findDijkstraPath(gameMap, pointA, enteredPointB);

                    if (path.empty()) {
                        ux.setConfirmationMessage("Dijkstra couldn't find a path! Try a different destination.");
                    } else {
                        gps.setPath(path, DIJKSTRA);
                        ux.setConfirmationMessage("Dijkstra path found with " + std::to_string(path.size()) + " steps.");
                    }
                }
                else if (event.key.code == sf::Keyboard::Num2) {
                    auto path = findAStarPath(gameMap, pointA, enteredPointB);

                    if (path.empty()) {
                        ux.setConfirmationMessage("A* couldn't find a path! Try a different destination.");
                    } else {
                        gps.setPath(path, ASTAR);
                        ux.setConfirmationMessage("A* path found with " + std::to_string(path.size()) + " steps.");
                    }
                }
                else if (event.key.code == sf::Keyboard::Num3) {
                    auto pathD = findDijkstraPath(gameMap, pointA, enteredPointB);
                    auto pathA = findAStarPath(gameMap, pointA, enteredPointB);

                    if (pathD.empty() && pathA.empty()) {
                        ux.setConfirmationMessage("Neither algorithm could find a path! Try a different destination.");
                    } else {
                        gps.setComparisonPaths(pathD, pathA);

                        std::stringstream ss;
                        ss << "Dijkstra: " << pathD.size() << " steps, A*: " << pathA.size() << " steps";
                        if (pathD.empty()) ss << " (Dijkstra failed)";
                        if (pathA.empty()) ss << " (A* failed)";
                        ux.setConfirmationMessage(ss.str());
                    }
                }
            }

            navigator.handleInput(sf::Time::Zero, event, view);
        }

        // Update game objects
        sf::Time frameTime = clock.restart();
        navigator.handleInput(frameTime, sf::Event(), view);
        navigator.update(frameTime);
        gameMap.updateLavaSpread(frameTime.asSeconds());

        gps.update(frameTime.asSeconds());
        view.setCenter(navigator.getPosition());
        window.setView(view);

        // Render frame
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