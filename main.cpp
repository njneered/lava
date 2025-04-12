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
    Background background("assets/welcomewindow1.jpeg");
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

    // Center the camera on the middle of the isometric map
    sf::View view;
    view.setSize(400, 300);
    view.setCenter(0,0);
    window.setView(view);

    Background gameBackground("assets/sky.jpeg");

    NaviGator navigator("sprites/navigator.png", sf::Vector2f(0.f, 0.f));
    sf::Clock clock;


    UX ux("The Floor is Lava!\n\nUse WASD to move.");


    // MAIN GAME
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
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