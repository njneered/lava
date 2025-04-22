#ifndef GPS_H
#define GPS_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "map.h" // Changed from chunkManager to map

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

    // Updated to use Map instead of ChunkManager
    void draw(sf::RenderWindow& window, const Map& map);

    void clearPath();
};

#endif