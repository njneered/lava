//
// Created by Nj on 4/10/2025.
//

#include "NaviGator.h"
#include <iostream>
#include <cmath>
using namespace std;

// function to normalize a 2d vector
sf::Vector2f normalize(const sf::Vector2f &v) {
    float len = std::sqrt(v.x * v.x + v.y * v.y);
    if (len != 0) {
        return sf::Vector2f(v.x / len, v.y / len);
    }
    return sf::Vector2f(0.f, 0.f);
}

NaviGator::NaviGator(const std::string &imagePath, const sf::Vector2f &startPosition)
    : navigatorSpeed(100.f), currentFrame(0), frameDuration(0.2f), animationTimer(0.f), isMoving(false) {

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

void NaviGator::handleInput(const sf::Time &frameTime, const sf::Event &event, sf::View &gameView) {
    sf::Vector2f input(0.f, 0.f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        input.y -= 1.f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        input.x -= 1.f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        input.y += 1.f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        input.x += 1.f;
    }

    // normalize input so diagonal movement is not faster
    if (input.x != 0.f || input.y != 0.f) {
        input = normalize(input);
    }

    // rotate input by 45 deg to convert from cartesian movement to isometric movement
    // since (cos(45) approx (45) approx 0.7071
    const float cos45 = 0.7071f;
    const float sin45 = 0.7071f;
    sf::Vector2f isometricMovement;
    isometricMovement.x = cos45 * input.x - sin45 * input.y;
    isometricMovement.y = sin45 * input.x + cos45 * input.y;
    isometricMovement *= navigatorSpeed * frameTime.asSeconds();

    navigatorSprite.move(isometricMovement);
    isMoving = (input.x != 0.f || input.y != 0.f);


    if (event.type == sf::Event::MouseWheelScrolled) {
        if (event.mouseWheelScroll.delta > 0) {
            gameView.zoom(0.9f); // Zoom in (reduce the view size by 10%).
        } else if (event.mouseWheelScroll.delta < 0) {
            gameView.zoom(1.1f); // Zoom out (increase the view size by 10%).
        }
    }
}

void NaviGator::update(const sf::Time &frameTime) {
    if (isMoving) {
        animationTimer += frameTime.asSeconds();
        if (animationTimer >= frameDuration) {
            animationTimer = 0.f;
            currentFrame = (currentFrame + 1) % frames.size();
            updateFrame();
        }
    }
    else {
        currentFrame = 0;
        updateFrame();
        animationTimer = 0.f;
    }
}

void NaviGator::draw(sf::RenderWindow &window) {
    window.draw(navigatorSprite);
}

sf::Vector2f NaviGator::getPosition() const {
    return navigatorSprite.getPosition();
}

void NaviGator::updateFrame() {
    navigatorSprite.setTextureRect(frames[currentFrame]);
}



