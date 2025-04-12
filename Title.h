//
// Created by Nj on 4/11/2025.
//

#ifndef TITLE_H
#define TITLE_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
using namespace std;


class Title {
    vector<sf::Text> lines;

    sf::Font font;
    bool finished;


public:
    Title(const std::string& message = "Welcome to The Floor is Lava!\n\n\n\n\n\n\n\n\n\n\n\n\n\nA Game by Reptile Dysfunction (Group 77)\n\n\n\n\n\n\n\n\n\n\n\n\n\nClick anywhere to continue.");

    void handleEvent(const sf::Event& event);

    void draw(sf::RenderWindow& window);

    bool isFinished() const;

};

#endif //TITLE_H
