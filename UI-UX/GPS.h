//
// Created by Nj on 4/17/2025.
//

#ifndef GPS_H
#define GPS_H

#include <SFML/Graphics.hpp>
#include <vector>

enum AlgorithmType {
    DIJKSTRA,
    ASTAR
};

class GPS {
    std::vector<sf::Vector2i> path;
    std::vector<sf::Vector2i> dijkstraPath, astarPath;

    AlgorithmType currentAlgo;

    float glowAlpha;
    float glowDirection;

    bool showingComparison = false;
public:
    GPS();

    void setPath(const std::vector<sf::Vector2i>& newPath, AlgorithmType algo);
    void setComparisonPaths(const std::vector<sf::Vector2i>& dPath, const std::vector<sf::Vector2i>& aPath);
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
    void clearPath();
};

#endif
