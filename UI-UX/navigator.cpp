#include "NaviGator.h"
#include <iostream>
#include <cmath>
using namespace std;

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

void NaviGator::handleInput(sf::Time deltaTime, const sf::Event& event, sf::View* view) {
    const float tileW = 32.f;
    const float tileH = 16.f;
    const float moveSpeed = navigatorSpeed * deltaTime.asSeconds();

    sf::Vector2f movement(0.f, 0.f);

    // Handle keyboard movement
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        movement.x += moveSpeed;
        movement.y -= moveSpeed;
        isMoving = true;
        facingLeft = false;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        movement.x -= moveSpeed;
        movement.y += moveSpeed;
        isMoving = true;
        facingLeft = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        movement.x -= moveSpeed;
        movement.y -= moveSpeed;
        isMoving = true;
        facingLeft = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        movement.x += moveSpeed;
        movement.y += moveSpeed;
        isMoving = true;
        facingLeft = false;
    }

    if (movement.x != 0.f || movement.y != 0.f) {
        navigatorSprite.move(movement);
    } else {
        isMoving = false;
    }
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
    sf::Vector2f pos = navigatorSprite.getPosition();
    navigatorSprite.setColor(sf::Color::White);
    navigatorSprite.setPosition(pos);

    if (facingLeft) {
        navigatorSprite.setScale(-1.f, 1.f);
        navigatorSprite.setOrigin(16.f, 16.f);
    } else {
        navigatorSprite.setScale(1.f, 1.f);
        navigatorSprite.setOrigin(16.f, 16.f);
    }

    window.draw(navigatorSprite);
    navigatorSprite.setPosition(pos);
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

void NaviGator::setPosition(const sf::Vector2f& pos) {
    navigatorSprite.setPosition(pos);
}