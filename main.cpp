#include <SFML/Graphics.hpp>
#include <iostream>
#include "Map.h"
#include "NaviGator.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "The Floor is Lava!!!");
    window.setFramerateLimit(60);

    Map gameMap;

    if (!gameMap.loadFromFile("maps/map.tmx", 32, 16)) {
        std::cerr << "Failed to load the map.\n";
        return 1;
    }

    // Center the camera on the middle of the isometric map
    sf::View view;
    view.setSize(400, 300);
    view.setCenter(0,0);
    window.setView(view);

    NaviGator navigator("sprites/navigator.png", sf::Vector2f(0.f, 0.f));

    float centerX = ((gameMap.getWidth() - gameMap.getHeight()) * (32 / 2)) / 2.0f;
    float centerY = ((gameMap.getWidth() + gameMap.getHeight()) * (16 / 2)) / 2.0f;

    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        sf::Time frameTime = clock.restart(); // calculate elapsed time since the last fram

        navigator.handleInput(frameTime);
        navigator.update(frameTime);

        view.setCenter(navigator.getPosition());
        window.setView(view);

        window.clear(sf::Color::Black);
        gameMap.draw(window);
        navigator.draw(window);
        window.display();
    }

    return 0;
}

