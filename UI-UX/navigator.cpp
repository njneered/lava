//
// Created by Nj on 4/10/2025.
//

#include "NaviGator.h"
#include <iostream>
#include <cmath>
using namespace std;

sf::Vector2f normalize(const sf::Vector2f &v) {
    float len = std::sqrt(v.x * v.x + v.y * v.y);
    if (len != 0) {
        return sf::Vector2f(v.x / len, v.y / len);
    }
    return sf::Vector2f(0.f, 0.f);
}

NaviGator::NaviGator(const std::string &imagePath, const sf::Vector2f &startPosition)
    : navigatorSpeed(15.f), currentFrame(0), frameDuration(0.2f), animationTimer(0.f), isMoving(false) {

    if (!navigatorTexture.loadFromFile(imagePath)) {
        cerr << "Failed to load " << imagePath << "\n";
    }
    navigatorSprite.setTexture(navigatorTexture);

    frames.push_back(sf::IntRect(0, 0, 32, 32)); // frame 0
    frames.push_back(sf::IntRect(0, 32, 32, 32)); // frame 1

    navigatorSprite.setOrigin(16, 16);
    navigatorSprite.setPosition(startPosition);

    navigatorSprite.setTextureRect(frames[currentFrame]);

}

void NaviGator::handleInput(sf::Time deltaTime, const sf::Event& event, const sf::View& view) {
    float moveSpeed = navigatorSpeed * deltaTime.asSeconds();

    const float tileW = 32.f;
    const float tileH = 16.f;

    sf::Vector2f offset(0.f, 0.f);
    isMoving = false;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        offset.x -= tileW / 2 * moveSpeed;
        offset.y += tileH / 2 * moveSpeed;
        isMoving = true;    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        offset.x += tileW / 2 * moveSpeed;
        offset.y -= tileH / 2 * moveSpeed;
        isMoving = true;    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        offset.x -= tileW / 2 * moveSpeed;
        offset.y -= tileH / 2 * moveSpeed;
        isMoving = true;
        facingLeft = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        offset.x += tileW / 2 * moveSpeed;
        offset.y += tileH / 2 * moveSpeed;
        isMoving = true;
        facingLeft = false;
    }

    navigatorSprite.move(offset);

}

void NaviGator::update(const sf::Time &frameTime) {
    if (isMoving) {
        animationTimer += frameTime.asSeconds();
        if (animationTimer >= frameDuration) {
            updateFrame();
            animationTimer = 0.f;
        }
    }
}


void NaviGator::draw(sf::RenderWindow &window) {
    if (facingLeft) {
        navigatorSprite.setScale(-1.f, 1.f);
        navigatorSprite.setOrigin(32.f, 0.f);
    } else {
        navigatorSprite.setScale(1.f, 1.f);
        navigatorSprite.setOrigin(0.f, 0.f);
    }

    window.draw(navigatorSprite);
}

sf::Vector2f NaviGator::getPosition() const {
    return navigatorSprite.getPosition();
}

void NaviGator::updateFrame() {
    currentFrame = (currentFrame + 1) % frames.size();
    navigatorSprite.setTextureRect(frames[currentFrame]);
}


void NaviGator::setScale(const sf::Vector2f &scale) {
    navigatorSprite.setScale(scale);
}


