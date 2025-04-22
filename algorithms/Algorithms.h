//
// Created by Nj on 4/17/2025.
//

#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <vector>
#include <SFML/System.hpp>
#include "../UI-UX/Map.h"
using namespace std;

vector<sf::Vector2i> findDijkstraPath(const Map& map, sf::Vector2i start, sf::Vector2i goal);
vector<sf::Vector2i> findAStarPath(const Map& map, sf::Vector2i start, sf::Vector2i goal);

#endif
