//
// Created by Nj on 4/10/2025.
//

#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
using namespace std;

class NaviGator {
    sf::Texture navigatorTexture;
    sf::Sprite navigatorSprite;

    vector<sf::IntRect> frames;
    int currentFrame;

    float frameDuration;
    float animationTimer;
    float navigatorSpeed;

    bool isMoving;
    bool facingLeft = false;

    void updateFrame();

public:
    NaviGator(const string& texturePath, const sf::Vector2f &startPosition);
    void handleInput(sf::Time deltaTime, const sf::Event& event, const sf::View& view);
    void update(const sf::Time &frameTime);
    void draw(sf::RenderWindow &window);
    void setScale(const sf::Vector2f &scale);
    sf::Vector2f getPosition() const;

};

#endif //NAVIGATOR_H
