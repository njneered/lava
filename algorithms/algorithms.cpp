#include "Algorithms.h"
#include <iostream>
#include <cmath>
#include <limits>
#include <algorithm>
#include "../terrain/constants.h" // For grid size constants
#include "../terrain/utility.h"

// Hash function for Vector2i in a priority queue
struct VectorPriorityQueueCompare {
    bool operator()(const std::pair<sf::Vector2i, float>& a, const std::pair<sf::Vector2i, float>& b) const {
        return a.second > b.second; // Min-heap for priority queue
    }
};

// Helper function to get neighbors
std::vector<sf::Vector2i> getNeighbors(const sf::Vector2i& pos, const Map& map) {
    std::vector<sf::Vector2i> neighbors;

    // Four possible directions (up, right, down, left)
    const int dx[] = {0, 1, 0, -1};
    const int dy[] = {-1, 0, 1, 0};

    for (int i = 0; i < 4; ++i) {
        sf::Vector2i next(pos.x + dx[i], pos.y + dy[i]);

        // Check if within grid bounds
        if (next.x >= 0 && next.x < Chunks::gridWidth &&
            next.y >= 0 && next.y < Chunks::gridHeight) {

            // Check if walkable (not lava or other obstacle)
            if (map.isWalkable(next.x, next.y)) {
                neighbors.push_back(next);
            }
        }
    }

    return neighbors;
}

// Manhattan distance heuristic for A*
float heuristic(const sf::Vector2i& a, const sf::Vector2i& b) {
    return static_cast<float>(std::abs(a.x - b.x) + std::abs(a.y - b.y));
}

// Dijkstra's algorithm implementation
std::vector<sf::Vector2i> findDijkstraPath(const Map& map, sf::Vector2i start, sf::Vector2i goal) {
    // Start and goal must be walkable
    if (!map.isWalkable(start.x, start.y) || !map.isWalkable(goal.x, goal.y)) {
        return std::vector<sf::Vector2i>();
    }

    // Priority queue for Dijkstra
    std::priority_queue<std::pair<sf::Vector2i, float>,
                        std::vector<std::pair<sf::Vector2i, float>>,
                        VectorPriorityQueueCompare> openSet;

    // Cost map and visited set
    std::unordered_map<sf::Vector2i, float, Vector2iHash> costSoFar;
    std::unordered_map<sf::Vector2i, sf::Vector2i, Vector2iHash> cameFrom;

    // Initialize with start node
    openSet.push({start, 0.0f});
    costSoFar[start] = 0.0f;
    cameFrom[start] = start; // Start came from itself (marker for reconstruction)

    bool foundPath = false;

    while (!openSet.empty()) {
        auto current = openSet.top().first;
        openSet.pop();

        // Check if we reached the goal
        if (current.x == goal.x && current.y == goal.y) {
            foundPath = true;
            break;
        }

        // Get valid neighbors
        auto neighbors = getNeighbors(current, map);

        for (const auto& next : neighbors) {
            // Cost is 1.0 for each step (uniform grid)
            float newCost = costSoFar[current] + 1.0f;

            // If we haven't visited this node or found a better path
            if (costSoFar.find(next) == costSoFar.end() || newCost < costSoFar[next]) {
                costSoFar[next] = newCost;
                cameFrom[next] = current;
                openSet.push({next, newCost});
            }
        }
    }

    // Reconstruct path if found
    std::vector<sf::Vector2i> path;
    if (foundPath) {
        sf::Vector2i current = goal;
        path.push_back(current);

        while (!(current.x == start.x && current.y == start.y)) {
            current = cameFrom[current];
            path.push_back(current);
        }

        // Reverse to get path from start to goal
        std::reverse(path.begin(), path.end());
    }

    return path;
}

// A* algorithm implementation
std::vector<sf::Vector2i> findAStarPath(const Map& map, sf::Vector2i start, sf::Vector2i goal) {
    // Start and goal must be walkable
    if (!map.isWalkable(start.x, start.y) || !map.isWalkable(goal.x, goal.y)) {
        return std::vector<sf::Vector2i>();
    }

    // Priority queue for A*
    std::priority_queue<std::pair<sf::Vector2i, float>,
                        std::vector<std::pair<sf::Vector2i, float>>,
                        VectorPriorityQueueCompare> openSet;

    // Cost maps and visited set
    std::unordered_map<sf::Vector2i, float, Vector2iHash> gScore; // Cost from start
    std::unordered_map<sf::Vector2i, float, Vector2iHash> fScore; // Estimated total cost
    std::unordered_map<sf::Vector2i, sf::Vector2i, Vector2iHash> cameFrom;

    // Initialize with start node
    openSet.push({start, 0.0f});
    gScore[start] = 0.0f;
    fScore[start] = heuristic(start, goal);
    cameFrom[start] = start; // Start came from itself (marker for reconstruction)

    bool foundPath = false;

    while (!openSet.empty()) {
        auto current = openSet.top().first;
        openSet.pop();

        // Check if we reached the goal
        if (current.x == goal.x && current.y == goal.y) {
            foundPath = true;
            break;
        }

        // Get valid neighbors
        auto neighbors = getNeighbors(current, map);

        for (const auto& next : neighbors) {
            // Cost is 1.0 for each step (uniform grid)
            float tentative_gScore = gScore[current] + 1.0f;

            // If we haven't visited this node or found a better path
            if (gScore.find(next) == gScore.end() || tentative_gScore < gScore[next]) {
                // Update path and costs
                cameFrom[next] = current;
                gScore[next] = tentative_gScore;
                fScore[next] = tentative_gScore + heuristic(next, goal);
                openSet.push({next, fScore[next]});
            }
        }
    }

    // Reconstruct path if found
    std::vector<sf::Vector2i> path;
    if (foundPath) {
        sf::Vector2i current = goal;
        path.push_back(current);

        while (!(current.x == start.x && current.y == start.y)) {
            current = cameFrom[current];
            path.push_back(current);
        }

        // Reverse to get path from start to goal
        std::reverse(path.begin(), path.end());
    }

    return path;
}