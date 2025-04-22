#include "GPS.h"
#include <cmath>
#include <iostream>

GPS::GPS()
    : glowAlpha(100),
      glowDirection(1),
      currentAlgo(DIJKSTRA),
      showingComparison(false) {
}

void GPS::setPath(const std::vector<sf::Vector2i>& newPath, AlgorithmType algo) {
    path = newPath;
    currentAlgo = algo;
    showingComparison = false;
}

void GPS::setComparisonPaths(const std::vector<sf::Vector2i>& dPath, const std::vector<sf::Vector2i>& aPath) {
    dijkstraPath = dPath;
    astarPath = aPath;
    showingComparison = true;
}

void GPS::update(float deltaTime) {
    // Animate the glow effect
    glowAlpha += glowDirection * 60 * deltaTime;
    if (glowAlpha >= 200) {
        glowAlpha = 200;
        glowDirection = -1;
    } else if (glowAlpha <= 60) {
        glowAlpha = 60;
        glowDirection = 1;
    }
}

void GPS::draw(sf::RenderWindow& window, const Map& map) {
    auto areAdjacent = [](const sf::Vector2i& a, const sf::Vector2i& b) -> bool {
        int dx = std::abs(a.x - b.x);
        int dy = std::abs(a.y - b.y);
        return dx + dy == 1;
    };

    auto drawSafePath = [&](const std::vector<sf::Vector2i>& pathToDraw, sf::Color color, bool glow) {
        if (pathToDraw.empty()) return;

        sf::Color pathColor = color;
        if (glow) {
            pathColor.a = static_cast<sf::Uint8>(glowAlpha);
        }

        const float tileWidth = 32.f;
        const float tileHeight = 16.f;

        const float offsetX = 0.f;
        const float offsetY = 0.f;

        for (const auto& tile : pathToDraw) {
            // Check if the tile is walkable using Map instead of ChunkManager
            if (!map.isWalkable(tile.x, tile.y)) {
                continue;
            }

            float x = (tile.x - tile.y) * (tileWidth / 2.f) + offsetX;
            float y = (tile.x + tile.y) * (tileHeight / 2.f) + offsetY;

            sf::CircleShape nodeDot(3.f);
            nodeDot.setOrigin(3.f, 3.f);
            nodeDot.setPosition(x, y);
            nodeDot.setFillColor(pathColor);
            window.draw(nodeDot);
        }

        for (size_t i = 1; i < pathToDraw.size(); ++i) {
            const auto& prev = pathToDraw[i-1];
            const auto& curr = pathToDraw[i];

            // Check walkability using Map
            if (!map.isWalkable(prev.x, prev.y) || !map.isWalkable(curr.x, curr.y)) {
                continue;
            }

            if (!areAdjacent(prev, curr)) {
                continue;
            }

            float x1 = (prev.x - prev.y) * (tileWidth / 2.f) + offsetX;
            float y1 = (prev.x + prev.y) * (tileHeight / 2.f) + offsetY;
            float x2 = (curr.x - curr.y) * (tileWidth / 2.f) + offsetX;
            float y2 = (curr.x + curr.y) * (tileHeight / 2.f) + offsetY;

            sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(x1, y1), pathColor),
                sf::Vertex(sf::Vector2f(x2, y2), pathColor)
            };
            window.draw(line, 2, sf::Lines);
        }

        if (!pathToDraw.empty()) {
            sf::CircleShape startMarker(6.f);
            startMarker.setOrigin(6.f, 6.f);
            float startX = (pathToDraw.front().x - pathToDraw.front().y) * (tileWidth / 2.f) + offsetX;
            float startY = (pathToDraw.front().x + pathToDraw.front().y) * (tileHeight / 2.f) + offsetY;
            startMarker.setPosition(startX, startY);
            startMarker.setFillColor(sf::Color::Green);
            window.draw(startMarker);

            sf::CircleShape endMarker(6.f);
            endMarker.setOrigin(6.f, 6.f);
            float endX = (pathToDraw.back().x - pathToDraw.back().y) * (tileWidth / 2.f) + offsetX;
            float endY = (pathToDraw.back().x + pathToDraw.back().y) * (tileHeight / 2.f) + offsetY;
            endMarker.setPosition(endX, endY);
            endMarker.setFillColor(sf::Color::Yellow);
            window.draw(endMarker);
        }
    };

    if (showingComparison) {
        drawSafePath(dijkstraPath, sf::Color(50, 170, 255, 200), false);
        drawSafePath(astarPath, sf::Color(0, 255, 150, 200), false);

        sf::RectangleShape background(sf::Vector2f(180, 50));
        background.setFillColor(sf::Color(0, 0, 0, 180));
        background.setPosition(10, 10);
        window.draw(background);

        sf::RectangleShape dijkstraIndicator(sf::Vector2f(20, 10));
        dijkstraIndicator.setFillColor(sf::Color(50, 170, 255));
        dijkstraIndicator.setPosition(20, 20);
        window.draw(dijkstraIndicator);

        sf::RectangleShape astarIndicator(sf::Vector2f(20, 10));
        astarIndicator.setFillColor(sf::Color(0, 255, 150));
        astarIndicator.setPosition(20, 40);
        window.draw(astarIndicator);
    } else {
        sf::Color pathColor = (currentAlgo == DIJKSTRA)
            ? sf::Color(50, 170, 255)
            : sf::Color(0, 255, 150);

        drawSafePath(path, pathColor, true);
    }
}

void GPS::clearPath() {
    path.clear();
    dijkstraPath.clear();
    astarPath.clear();
    showingComparison = false;
}