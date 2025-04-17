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
    sf::Texture buttonsTexture;
    sf::Sprite redButton;
    sf::Sprite greenButton;

public:
    UX(const string& message);
    void setMessage(const string& message);
    void draw(sf::RenderWindow& window);
    void initializeButtons();
};

#endif //UX_H
