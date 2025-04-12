//
// Created by Nj on 4/12/2025.
//

#ifndef UX_H
#define UX_H

#include <SFML/Graphics.hpp>
#include <string>
using namespace std;

class UX {
    sf::Font font;
    sf::Text text;

public:
    UX(const string& message);
    void setMessage(const string& message);
    void draw(sf::RenderWindow& window);
};

#endif //UX_H
