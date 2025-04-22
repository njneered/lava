#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <SFML/System/Vector2.hpp>
#include "map.h" // Changed from chunkManager to map

// Dijkstra's algorithm implementation
std::vector<sf::Vector2i> findDijkstraPath(const Map& map, sf::Vector2i start, sf::Vector2i goal);

// A* algorithm implementation
std::vector<sf::Vector2i> findAStarPath(const Map& map, sf::Vector2i start, sf::Vector2i goal);

#endif // ALGORITHMS_H