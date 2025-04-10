//
// Created by Nj on 4/10/2025.
//

#include "NaviGator.h"
#include <iostream>
using namespace std;

NaviGator::NaviGator(const std::string &imagePath, const sf::Vector2f &startPosition)
    : navigatorSpeed(100.f) {

    navigatorSprite.setTexture(navigatorTexture);
    navigatorSprite.setOrigin(navigatorTexture.getSize().x / 2.0f, navigatorTexture.getSize().y / 2.0f);
    navigatorSprite.setPosition(startPosition);

}

void NaviGator::handleInput(const sf::Time &frameTime, const sf::Event &event, sf::View &gameView) {
    sf::Vector2f moveDirection(0.f, 0.f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        moveDirection.y -= navigatorSpeed * frameTime.asSeconds();
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        moveDirection.x -= navigatorSpeed * frameTime.asSeconds();
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        moveDirection.y += navigatorSpeed * frameTime.asSeconds();
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        moveDirection.x += navigatorSpeed * frameTime.asSeconds();
    }

    navigatorSprite.move(moveDirection);

    if (event.type == sf::Event::MouseWheelScrolled) {
        if (event.mouseWheelScroll.delta > 0) {
            gameView.zoom(0.9f); // Zoom in (reduce the view size by 10%).
        } else if (event.mouseWheelScroll.delta < 0) {
            gameView.zoom(1.1f); // Zoom out (increase the view size by 10%).
        }
    }
}

void NaviGator::update(const sf::Time &frameTime) {
    // implement later
}

void NaviGator::draw(sf::RenderWindow &window) {
    window.draw(navigatorSprite);
}

sf::Vector2f NaviGator::getPosition() const {
    return navigatorSprite.getPosition();
}



