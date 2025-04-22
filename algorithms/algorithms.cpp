#include "Algorithms.h"
#include <queue>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <iostream>

using namespace std;

// This hash function definition is also in LavaGenerator.h, but we need it here too
struct Vector2iHash1 {
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
    cout << "Finding Dijkstra path from (" << start.x << "," << start.y
         << ") to (" << goal.x << "," << goal.y << ")" << endl;

    // Check if start or goal is unwalkable
    if (!map.isWalkable(start.x, start.y) || !map.isWalkable(goal.x, goal.y)) {
        cout << "Start or goal is not walkable!" << endl;
        return vector<sf::Vector2i>(); // Return empty path
    }

    priority_queue<pair<int, sf::Vector2i>, vector<pair<int, sf::Vector2i>>, CompareCost> frontier;
    frontier.push({0, start});

    unordered_map<sf::Vector2i, sf::Vector2i, Vector2iHash1> cameFrom;
    unordered_map<sf::Vector2i, int, Vector2iHash1> costSoFar;

    cameFrom[start] = start;
    costSoFar[start] = 0;

    vector<sf::Vector2i> directions = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1}
    };

    int nodesExplored = 0;
    bool pathFound = false;

    while (!frontier.empty()) {
        sf::Vector2i current = frontier.top().second;
        frontier.pop();
        nodesExplored++;

        if (current == goal) {
            pathFound = true;
            break;
        }

        for (const auto& dir : directions) {
            sf::Vector2i next = current + dir;

            // Skip if out of bounds or unwalkable (e.g., lava)
            if (next.x < 0 || next.y < 0 || next.x >= map.getWidth() || next.y >= map.getHeight() ||
                !map.isWalkable(next.x, next.y)) {
                continue;

            if (map.isLava(next.x, next.y))
                continue;
            }

            int newCost = costSoFar[current] + 1;

            if (costSoFar.find(next) == costSoFar.end() || newCost < costSoFar[next]) {
                costSoFar[next] = newCost;
                frontier.push({newCost, next});
                cameFrom[next] = current;
            }
        }
    }

    vector<sf::Vector2i> path;
    if (!pathFound) {
        cout << "Dijkstra: No path found after exploring " << nodesExplored << " nodes" << endl;
        return path;
    }

    sf::Vector2i current = goal;
    while (current != start) {
        path.push_back(current);
        current = cameFrom[current];
    }

    path.push_back(start);
    reverse(path.begin(), path.end());

    cout << "Dijkstra: Path found with " << path.size() << " steps after exploring "
         << nodesExplored << " nodes" << endl;

    return path;
}

vector<sf::Vector2i> findAStarPath(const Map& map, sf::Vector2i start, sf::Vector2i goal) {
    cout << "Finding A* path from (" << start.x << "," << start.y
         << ") to (" << goal.x << "," << goal.y << ")" << endl;

    // Check if start or goal is unwalkable
    if (!map.isWalkable(start.x, start.y) || !map.isWalkable(goal.x, goal.y)) {
        cout << "Start or goal is not walkable!" << endl;
        return vector<sf::Vector2i>(); // Return empty path
    }

    priority_queue<pair<int, sf::Vector2i>, vector<pair<int, sf::Vector2i>>, CompareCost> frontier;
    int startPriority = heuristic(start, goal);
    frontier.push({startPriority, start});

    unordered_map<sf::Vector2i, sf::Vector2i, Vector2iHash1> cameFrom;
    unordered_map<sf::Vector2i, int, Vector2iHash1> costSoFar;

    cameFrom[start] = start;
    costSoFar[start] = 0;

    vector<sf::Vector2i> directions = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1}
    };

    int nodesExplored = 0;
    bool pathFound = false;

    while (!frontier.empty()) {
        sf::Vector2i current = frontier.top().second;
        frontier.pop();
        nodesExplored++;

        if (current == goal) {
            pathFound = true;
            break;
        }

        for (const auto& dir : directions) {
            sf::Vector2i next = current + dir;

            // Skip if out of bounds or unwalkable (e.g., lava)
            if (next.x < 0 || next.y < 0 || next.x >= map.getWidth() || next.y >= map.getHeight() ||
                !map.isWalkable(next.x, next.y)) {
                continue;
            }

            int newCost = costSoFar[current] + 1;

            if (costSoFar.find(next) == costSoFar.end() || newCost < costSoFar[next]) {
                costSoFar[next] = newCost;
                int priority = newCost + heuristic(next, goal);
                frontier.push({priority, next});
                cameFrom[next] = current;
            }
        }
    }

    vector<sf::Vector2i> path;
    if (!pathFound) {
        cout << "A*: No path found after exploring " << nodesExplored << " nodes" << endl;
        return path;
    }

    sf::Vector2i current = goal;
    while (current != start) {
        path.push_back(current);
        current = cameFrom[current];
    }

    path.push_back(start);
    reverse(path.begin(), path.end());

    cout << "A*: Path found with " << path.size() << " steps after exploring "
         << nodesExplored << " nodes" << endl;

    return path;
}