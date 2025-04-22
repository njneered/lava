#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <../include/SFML/Graphics.hpp>
#include <string>
using namespace std;

class Background {
    sf::Texture texture;
    sf::Sprite sprite;

public:
    Background(const string& imagePath);
    void setScale(float scaleX, float scaleY);
    void draw(sf::RenderWindow& window);
};

#endif //BACKGROUND_H
