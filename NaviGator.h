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
    float navigatorSpeed; // pixels per second

    vector<sf::IntRect> frames;
    int currentFrame;
    float frameDuration; // how long each frame stays in seconds
    float animationTimer; // accumulates elapsed time
    bool isMoving;

    void updateFrame();

public:
    NaviGator(const string& texturePath, const sf::Vector2f &startPosition);
    void handleInput(const sf::Time &frameTime, const sf::Event &event, sf::View &gameView);
    void update(const sf::Time &frameTime);
    void draw(sf::RenderWindow &window);
    sf::Vector2f getPosition() const;

    void setScale(const sf::Vector2f &scale); // set scale for main character sprite
};

#endif //NAVIGATOR_H
