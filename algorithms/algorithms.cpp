//
// Created by Nj on 4/17/2025.
//
#include "Algorithms.h"
#include <queue>
#include <unordered_map>
#include <cmath>
#include <algorithm>

using namespace std;

struct Vector2iHash {
    size_t operator()(const sf::Vector2i& v) const {
        size_t xHash = hash<int>()(v.x);
        size_t yHash = hash<int>()(v.y);
        return xHash ^ (yHash << 1);
    }
};

int heuristic(const sf::Vector2i& a, const sf::Vector2i& b) {
    return abs(a.x - b.x) + abs(a.y - b.y); // Manhattan distance
}

struct CompareCost {
    bool operator()(const pair<int, sf::Vector2i>& a, const pair<int, sf::Vector2i>& b) {
        return a.first > b.first; // Min-heap based on cost
    }
};

vector<sf::Vector2i> findDijkstraPath(const Map& map, sf::Vector2i start, sf::Vector2i goal) {
    priority_queue<pair<int, sf::Vector2i>, vector<pair<int, sf::Vector2i>>, CompareCost> frontier;
    frontier.push({0, start});

    unordered_map<sf::Vector2i, sf::Vector2i, Vector2iHash> cameFrom;
    unordered_map<sf::Vector2i, int, Vector2iHash> costSoFar;

    cameFrom[start] = start;
    costSoFar[start] = 0;

    vector<sf::Vector2i> directions = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1}
    };

    while (!frontier.empty()) {
        sf::Vector2i current = frontier.top().second;
        frontier.pop();

        if (current == goal) break;

        for (const auto& dir : directions) {
            sf::Vector2i next = current + dir;

            if (next.x < 0 || next.y < 0 || next.x >= map.getWidth() || next.y >= map.getHeight())
                continue;

            if (map.isLava(next.x, next.y))
                continue;

            int newCost = costSoFar[current] + 1;

            if (!costSoFar.count(next) || newCost < costSoFar[next]) {
                costSoFar[next] = newCost;
                frontier.push({newCost, next});
                cameFrom[next] = current;
            }
        }
    }

    vector<sf::Vector2i> path;
    if (!cameFrom.count(goal)) return path;

    sf::Vector2i current = goal;
    while (current != start) {
        path.push_back(current);
        current = cameFrom[current];
    }

    path.push_back(start);
    reverse(path.begin(), path.end());
    return path;
}

vector<sf::Vector2i> findAStarPath(const Map& map, sf::Vector2i start, sf::Vector2i goal) {
    priority_queue<pair<int, sf::Vector2i>, vector<pair<int, sf::Vector2i>>, CompareCost> frontier;
    frontier.push({0, start});

    unordered_map<sf::Vector2i, sf::Vector2i, Vector2iHash> cameFrom;
    unordered_map<sf::Vector2i, int, Vector2iHash> costSoFar;

    cameFrom[start] = start;
    costSoFar[start] = 0;

    vector<sf::Vector2i> directions = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1}
    };

    while (!frontier.empty()) {
        sf::Vector2i current = frontier.top().second;
        frontier.pop();

        if (current == goal) break;

        for (const auto& dir : directions) {
            sf::Vector2i next = current + dir;

            if (next.x < 0 || next.y < 0 || next.x >= map.getWidth() || next.y >= map.getHeight())
                continue;

            int newCost = costSoFar[current] + 1;

            if (!costSoFar.count(next) || newCost < costSoFar[next]) {
                costSoFar[next] = newCost;
                int priority = newCost + heuristic(next, goal);
                frontier.push({priority, next});
                cameFrom[next] = current;
            }
        }
    }

    vector<sf::Vector2i> path;
    if (!cameFrom.count(goal)) return path;

    sf::Vector2i current = goal;
    while (current != start) {
        path.push_back(current);
        current = cameFrom[current];
    }

    path.push_back(start);
    reverse(path.begin(), path.end());
    return path;
}
