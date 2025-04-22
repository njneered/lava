//
// Created by Nj on 4/17/2025.
//

#include "GPS.h"
#include <cmath>


GPS::GPS() : glowAlpha(100), glowDirection(1), currentAlgo(DIJKSTRA) {}

void GPS::setPath(const std::vector<sf::Vector2i>& newPath, AlgorithmType algo) {
    path = newPath;
    currentAlgo = algo;
    showingComparison = false;
}


std::vector<sf::Vector2i> dijkstraPath, astarPath;
bool showingComparison = false;

void GPS::setComparisonPaths(const std::vector<sf::Vector2i>& dPath, const std::vector<sf::Vector2i>& aPath) {
    dijkstraPath = dPath;
    astarPath = aPath;
    showingComparison = true;
}

void GPS::update(float deltaTime) {
    glowAlpha += glowDirection * 60 * deltaTime;
    if (glowAlpha >= 200) {
        glowAlpha = 200;
        glowDirection = -1;
    } else if (glowAlpha <= 60) {
        glowAlpha = 60;
        glowDirection = 1;
    }
}

void GPS::draw(sf::RenderWindow& window) {
    if (showingComparison) {
        auto drawPath = [&](const std::vector<sf::Vector2i>& path, sf::Color color) {
            for (size_t i = 1; i < path.size(); ++i) {
                sf::Vector2i a = path[i - 1];
                sf::Vector2i b = path[i];

                float ax = (a.x - a.y) * 16.f;
                float ay = (a.x + a.y) * 8.f;
                float bx = (b.x - b.y) * 16.f;
                float by = (b.x + b.y) * 8.f;

                sf::Vector2f dir(bx - ax, by - ay);
                float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);
                float rotation = std::atan2(dir.y, dir.x) * 180.f / 3.14159f;

                sf::RectangleShape segment(sf::Vector2f(length, 6.f));
                segment.setFillColor(color);
                segment.setOrigin(0, 3.f);
                segment.setPosition(ax, ay);
                segment.setRotation(rotation);
                window.draw(segment);
            }

            if (!path.empty()) {
                auto drawMarker = [&](sf::Vector2i tile, sf::Color col) {
                    float x = (tile.x - tile.y) * 16.f;
                    float y = (tile.x + tile.y) * 8.f;
                    sf::CircleShape marker(6.f);
                    marker.setFillColor(col);
                    marker.setOrigin(6.f, 6.f);
                    marker.setPosition(x, y);
                    window.draw(marker);
                };
                drawMarker(path.front(), sf::Color::Red);
                drawMarker(path.back(), sf::Color::Yellow);
            }
        };

        drawPath(dijkstraPath, sf::Color(50, 170, 255));  // 🔵 Brighter blue
        drawPath(astarPath,    sf::Color(0, 255, 150));   // 🟢 Brighter green

        return;
    }

    if (path.empty()) return;

    sf::Color pathColor = (currentAlgo == DIJKSTRA)
        ? sf::Color(50, 170, 255, static_cast<sf::Uint8>(glowAlpha))
        : sf::Color(0, 255, 150, static_cast<sf::Uint8>(glowAlpha));

    for (size_t i = 1; i < path.size(); ++i) {
        sf::Vector2i a = path[i - 1];
        sf::Vector2i b = path[i];

        float ax = (a.x - a.y) * 16.f;
        float ay = (a.x + a.y) * 8.f;
        float bx = (b.x - b.y) * 16.f;
        float by = (b.x + b.y) * 8.f;

        sf::Vector2f dir(bx - ax, by - ay);
        float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        float rotation = std::atan2(dir.y, dir.x) * 180.f / 3.14159f;

        sf::RectangleShape glow(sf::Vector2f(length + 4.f, 8.f));
        sf::Color glowColor = pathColor;
        glowColor.a = glowAlpha / 1.5;
        glow.setFillColor(glowColor);
        glow.setOrigin(2.f, 4.f);
        glow.setPosition(ax, ay);
        glow.setRotation(rotation);
        window.draw(glow);

        sf::RectangleShape segment(sf::Vector2f(length, 6.f));
        segment.setFillColor(pathColor);
        segment.setOrigin(0, 3.f);
        segment.setPosition(ax, ay);
        segment.setRotation(rotation);
        window.draw(segment);
    }

    if (!path.empty()) {
        float startX = (path.front().x - path.front().y) * 16.f;
        float startY = (path.front().x + path.front().y) * 8.f;
        float endX   = (path.back().x - path.back().y) * 16.f;
        float endY   = (path.back().x + path.back().y) * 8.f;

        sf::CircleShape startMarker(6.f);
        startMarker.setFillColor(sf::Color::Red);
        startMarker.setOrigin(6.f, 6.f);
        startMarker.setPosition(startX, startY);
        window.draw(startMarker);

        sf::CircleShape endMarker(6.f);
        endMarker.setFillColor(sf::Color::Yellow);
        endMarker.setOrigin(6.f, 6.f);
        endMarker.setPosition(endX, endY);
        window.draw(endMarker);
    }
}

void GPS::clearPath() {
    path.clear();
    dijkstraPath.clear();
    astarPath.clear();
    showingComparison = false;
}
