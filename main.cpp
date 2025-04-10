#include <SFML/Graphics.hpp>
#include <iostream>
#include "Map.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "The Floor is Lava!");
    window.setFramerateLimit(60);

    Map gameMap;

    if (!gameMap.loadFromFile("maps/map.tmx", 32, 16)) {
        std::cerr << "Failed to load the map.\n";
        return 1;
    }

    // Center the camera on the middle of the isometric map
    sf::View view;
    view.setSize(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y));

    float centerX = ((gameMap.getWidth() - gameMap.getHeight()) * (32 / 2)) / 2.0f;
    float centerY = ((gameMap.getWidth() + gameMap.getHeight()) * (16 / 2)) / 2.0f;

    view.setCenter(centerX, centerY);
    window.setView(view);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Black);
        gameMap.draw(window);
        window.display();
    }

    return 0;
}

