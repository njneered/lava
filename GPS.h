//
// Created by Nj on 4/17/2025.
//

#ifndef GPS_H
#define GPS_H

#include <SFML/Graphics.hpp>
#include <vector>

using namespace std;

enum AlgorithmType {
    DIJKSTRA,
    ASTAR
};

class GPS {
    vector<sf::Vector2i> path;
    AlgorithmType currentAlgorithm;
    float glowAlpha;
    float glowDirection;
    sf::Color getPathColor() const;

public:
    GPS();

    void setPath(const vector<sf::Vector2i>& path, AlgorithmType algorithm);
    void update(float deltaTime); // call each frame to animate glow
    void draw(sf::RenderWindow& window);
};

#endif //GPS_H
