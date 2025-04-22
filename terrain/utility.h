#ifndef UTILITY_H
#define UTILITY_H

#include <SFML/System.hpp>

struct Vector2iHash {
    std::size_t operator()(const sf::Vector2i& v) const {
        std::size_t xHash = std::hash<int>()(v.x);
        std::size_t yHash = std::hash<int>()(v.y);
        return xHash ^ (yHash << 1); // Combine hashes
    }
};

#endif