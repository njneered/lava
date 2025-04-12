//
// Created by Nj on 4/12/2025.
//

#include "UX.h"
#include <iostream>
using namespace std;

UX::UX(const string& message) {
    if (!font.loadFromFile("font/aesymatt.ttf")) {
        cerr << "Failed to load font" << endl;
    }
    text.setFont(font);
    text.setString(message);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color(227, 125, 77));
    text.setPosition(10.f, 10.f);
}

void UX::setMessage(const string &message) {
    text.setString(message);
}

void UX::draw(sf::RenderWindow &window) {
    sf::Vector2u size = window.getSize();
    float windowWidth = static_cast<float>(size.x);
    float windowHeight = static_cast<float>(size.y);

    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width / 2.0f, 0.f);
    text.setPosition(windowWidth/2.0f, 20.f);

    window.draw(text);
}

