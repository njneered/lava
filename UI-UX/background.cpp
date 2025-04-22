//
// Created by Nj on 4/11/2025.
//

#include "Background.h"
#include <iostream>
using namespace std;

Background::Background(const string &imagePath) {
    if(!texture.loadFromFile(imagePath)) {
        cerr << "Failed to load background image: " <<imagePath << endl;
    }
    sprite.setTexture(texture);
    sf::Vector2u size = texture.getSize();
    float scaleX = 1280.f / static_cast<float>(size.x);
    float scaleY = 720.f / static_cast<float>(size.y);
    sprite.setScale(scaleX, scaleY);
}

void Background::setScale(float scaleX, float scaleY) {
    sprite.setScale(scaleX, scaleY);
}

void Background::draw(sf::RenderWindow &window) {
    window.draw(sprite);
}

