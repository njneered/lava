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
    sprite.setScale(1280.f/texture.getSize().x, 720.f/texture.getSize().y);
}

void Background::setScale(float scaleX, float scaleY) {
    sprite.setScale(scaleX, scaleY);
}

void Background::draw(sf::RenderWindow &window) {
    window.draw(sprite);
}

