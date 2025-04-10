//
// Created by Nj on 4/10/2025.
//

#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
using namespace std;

class NaviGator {
    sf::Texture navigatorTexture;
    sf::Sprite navigatorSprite;
    float navigatorSpeed; // pixels per second

public:
    NaviGator(const string& texturePath, const sf::Vector2f &startPosition);
    void handleInput(const sf::Time &frameTime);
    void update(const sf::Time &frameTime);
    void draw(sf::RenderWindow &window);
    sf::Vector2f getPosition() const;

};

#endif //NAVIGATOR_H
