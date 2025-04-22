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

    // Fixed: Changed default parameter to avoid reference binding issue
    void handleInput(sf::Time deltaTime, const sf::Event &event = sf::Event(), sf::View *view = nullptr);

    void update(const sf::Time &frameTime);
    void draw(sf::RenderWindow &window);
    void setScale(const sf::Vector2f &scale);
    sf::Vector2f getPosition() const;
    void setPosition(const sf::Vector2f& pos);
};

#endif //NAVIGATOR_H