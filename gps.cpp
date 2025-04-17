//
// Created by Nj on 4/17/2025.
//

#include "GPS.h"
#include <cmath>
using namespace std;

GPS::GPS() :glowAlpha(100), glowDirection(1), currentAlgorithm(DIJKSTRA){}

void GPS::setPath(const vector<sf::Vector2i> &newPath, AlgorithmType algo) {
    path = newPath;
    currentAlgorithm = algo;
}

void GPS::update(float deltaTime) {
    glowAlpha += glowDirection * 60 * deltaTime; // this is for the pulsing effect
    if (glowAlpha >= 200) {
        glowAlpha = 200;
        glowDirection = -1;
    }

    else if(glowAlpha <= 60) {
        glowAlpha = 60;
        glowDirection = 1;
    }
}

sf::Color GPS::getPathColor() const {
    if (currentAlgorithm == DIJKSTRA) {
        return sf::Color(0, 120, 255, static_cast<sf::Uint8>(glowAlpha)); // DIJSKTRAS BLUE

    }
    else {
        return sf::Color(0, 255, 0, static_cast<sf::Uint8>(glowAlpha)); // ASTAR GREEN
    }
}

void GPS::draw(sf::RenderWindow& window) {
    if (path.size() < 2) return;

    sf::Color pathColor = getPathColor();

    // Draw glowing path segments between tiles
    for (size_t i = 1; i < path.size(); ++i) {
        sf::Vector2i a = path[i - 1];
        sf::Vector2i b = path[i];

        float ax = (a.x - a.y) * 16.f;
        float ay = (a.x + a.y) * 8.f;
        float bx = (b.x - b.y) * 16.f;
        float by = (b.x + b.y) * 8.f;

        sf::Vector2f dir(bx - ax, by - ay);
        float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);

        sf::RectangleShape segment(sf::Vector2f(length, 6.f));
        segment.setFillColor(pathColor);
        segment.setOrigin(0, 3.f);
        segment.setPosition(ax, ay);
        segment.setRotation(std::atan2(dir.y, dir.x) * 180.f / 3.14159f);

        window.draw(segment);
    }

    // Draw start and end markers
    auto drawMarker = [](sf::RenderWindow& window, sf::Vector2i tile, sf::Color color) {
        const float tileW = 32.f;
        const float tileH = 16.f;
        float isoX = (tile.x - tile.y) * (tileW / 2.f);
        float isoY = (tile.x + tile.y) * (tileH / 2.f);

        sf::CircleShape marker(6.f);  // radius
        marker.setFillColor(color);
        marker.setOrigin(6.f, 6.f);  // center it
        marker.setPosition(isoX, isoY);
        window.draw(marker);
    };

    drawMarker(window, path.front(), sf::Color::Red);     // Start marker
    drawMarker(window, path.back(),  sf::Color::Yellow);
}