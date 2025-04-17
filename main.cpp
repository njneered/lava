#include <SFML/Graphics.hpp>
#include <iostream>

#include "Background.h"
#include "Map.h"
#include "NaviGator.h"
#include "Title.h"
#include "Soundtrack.h"
#include "UX.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "The Floor is Lava!!!");
    window.setFramerateLimit(60);


    // MUSIC
    Soundtrack soundtrack;
    if (!soundtrack.loadFromFile("assets/soundtrack.mp3")) {
        cerr << "Failed to load soundtrack" << endl;
    }
    soundtrack.play();


    // TITLE SCREEN
    Background background("assets/sky.jpeg");
    Title title;

    while (window.isOpen() && !title.isFinished()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            title.handleEvent(event);
        }


        window.clear();
        window.setView(window.getDefaultView());
        background.draw(window);
        title.draw(window);
        window.display();
    }


    // GAME
    Map gameMap;

    if (!gameMap.loadFromFile("maps/map.tmx", 32, 16)) {
        std::cerr << "Failed to load the map.\n";
        return 1;
    }

    // CAMERA
    sf::View view;
    view.setSize(400, 300);
    view.setCenter(0,0);
    window.setView(view);

    Background gameBackground("assets/sky.jpeg");
    NaviGator navigator("sprites/navigator.png", sf::Vector2f(0.f, 0.f));
    sf::Clock clock;
    UX ux("The Floor is Lava!\nUse WASD to move.");

    sf::Vector2i pointB(-1, -1); // stores the user's selected destination tile which is called Point B

    // MAIN GAME
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
                sf::Vector2f worldPos = window.mapPixelToCoords(mousePixel);

                float tempX = (worldPos.y / 8.0f + worldPos.x / 16.0f) / 2.0f;
                float tempY = (worldPos.y / 8.0f - worldPos.x / 16.0f) / 2.0f;
                int tileX = static_cast<int>(tempX);
                int tileY = static_cast<int>(tempY);


                if (tileX >= 0 && tileY >= 0 && tileX < gameMap.getWidth() && tileY < gameMap.getHeight()) {
                    pointB = sf::Vector2i(tileX, tileY);
                    std::cout << "You clicked on tile (" << tileX << ", " << tileY << ") to set Point B.\n";
                }
            }

            navigator.handleInput(sf::Time::Zero, event, view);
        }


        sf::Time frameTime = clock.restart(); // calculate elapsed time since the last fram
        sf::Event dummyEvent; // dummy event that doesnt trigger anything
        dummyEvent.type = sf::Event::Count;
        navigator.handleInput(frameTime, dummyEvent, view);
        navigator.update(frameTime);
        view.setCenter(navigator.getPosition());
        window.setView(view);

        window.clear(sf::Color::Black);
        window.setView(window.getDefaultView());
        gameBackground.draw(window);

        window.setView(view);
        gameMap.draw(window);
        navigator.draw(window);


        if (pointB.x != -1 && pointB.y != -1) { // MARKER FOR POINT B
            sf::CircleShape marker(6);
            marker.setFillColor(sf::Color::Yellow);
            marker.setOrigin(6, 6);

            float isoX = (pointB.x - pointB.y) * 16.0f;
            float isoY = (pointB.x + pointB.y) * 8.0f;
            marker.setPosition(isoX, isoY);

            window.draw(marker);
        }

        window.setView(window.getDefaultView());
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
 */