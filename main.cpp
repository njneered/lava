#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <cmath>

#include "Background.h"
#include "Map.h"
#include "NaviGator.h"
#include "Title.h"
#include "Soundtrack.h"
#include "UX.h"
#include "GPS.h"

// Dummy pathfinding (replace with A* or Dijkstra later)
std::vector<sf::Vector2i> findPath(Map& map, sf::Vector2i start, sf::Vector2i goal) {
    std::vector<sf::Vector2i> path;
    sf::Vector2i current = start;
    int dx = (goal.x > start.x) ? 1 : -1;
    int dy = (goal.y > start.y) ? 1 : -1;

    while (current != goal) {
        if (current.x != goal.x) current.x += dx;
        else if (current.y != goal.y) current.y += dy;
        path.push_back(current);
    }
    path.push_back(goal); // include goal
    return path;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "The Floor is Lava!!!");
    window.setFramerateLimit(60);

    // --- Soundtrack ---
    Soundtrack soundtrack;
    if (!soundtrack.loadFromFile("assets/soundtrack.mp3")) {
        std::cerr << "Failed to load soundtrack" << std::endl;
    }
    soundtrack.play();

    // --- Title screen ---
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

    // --- Game setup ---
    Map gameMap;
    if (!gameMap.loadFromFile("maps/map.tmx", 32, 16)) {
        std::cerr << "Failed to load the map.\n";
        return 1;
    }

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
    ux.setSecondaryMessage("Press 1 for Dijkstra's, and 2 for A*");
    ux.setInstructionMessage("Click the textbox below and type tile coordinates (e.g., 65 9)");

    sf::Vector2i pointB(-1, -1);
    GPS gps;
    AlgorithmType currentAlgo = DIJKSTRA;

    // --- Main game loop ---
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            // Handle UI textbox typing
            ux.handleEvent(event, window);

            // Confirm typed coordinates with Enter
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter && ux.hasValidInput()) {
                pointB = ux.getTypedCoordinates();
                std::cout << "📍 Set Point B to (" << pointB.x << ", " << pointB.y << ")\n";

                float ax = (navigator.getPosition().y / 8.0f + navigator.getPosition().x / 16.0f) / 2.0f;
                float ay = (navigator.getPosition().y / 8.0f - navigator.getPosition().x / 16.0f) / 2.0f;
                sf::Vector2i pointA(static_cast<int>(ax), static_cast<int>(ay));

                std::vector<sf::Vector2i> path = findPath(gameMap, pointA, pointB);
                gps.setPath(path, currentAlgo);
                ux.resetTypedCoordinates();
            }

            // Toggle algorithm
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Num1) {
                    currentAlgo = DIJKSTRA;
                    std::cout << "Switched to Dijkstra's Algorithm\n";
                } else if (event.key.code == sf::Keyboard::Num2) {
                    currentAlgo = ASTAR;
                    std::cout << "Switched to A* Algorithm\n";
                }
            }

            navigator.handleInput(sf::Time::Zero, event, view);
        }

        // Update logic
        sf::Time frameTime = clock.restart();
        navigator.handleInput(frameTime, sf::Event(), view);
        navigator.update(frameTime);
        gps.update(frameTime.asSeconds());

        view.setCenter(navigator.getPosition());
        window.setView(view);

        // Drawing
        window.clear(sf::Color::Black);
        window.setView(window.getDefaultView());
        gameBackground.draw(window);

        window.setView(view);
        gameMap.draw(window);
        gps.draw(window);
        navigator.draw(window);

        window.setView(window.getDefaultView());
        ux.updateCursor();
        ux.draw(window); // includes textbox and messages
        window.display();
    }

    return 0;
}


/*
 * MUSIC:
* Music from #Uppbeat (free for Creators!):
https://uppbeat.io/t/color-parade/pixel-playground
License code: YXK8LCR0AWJD0S3M
 */