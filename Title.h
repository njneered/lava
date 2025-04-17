//
// Created by Nj on 4/11/2025.
//

#ifndef TITLE_H
#define TITLE_H

#include <SFML/Graphics.hpp>
#include <string>
using namespace std;


class Title {
    sf::Text titleText;
    sf::Text subtitleText;
    sf::Text instructionText;
    sf::Font font;
    bool finished;


public:
    Title();

    void handleEvent(const sf::Event& event);

    void draw(sf::RenderWindow& window);

    bool isFinished() const;

};

#endif //TITLE_H
