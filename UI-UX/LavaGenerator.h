#ifndef LAVA_GENERATOR_H
#define LAVA_GENERATOR_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <unordered_map>
#include "../UI-UX/perlin.h" // Using the PerlinNoise library

// For Vector2i hash function
struct Vector2iHash {
    std::size_t operator()(const sf::Vector2i& v) const {
        return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1);
    }
};

class LavaGenerator {
private:
    // Perlin noise generator
    siv::PerlinNoise perlin;

    // Lava map (true = lava, false = safe)
    std::unordered_map<sf::Vector2i, bool, Vector2iHash> lavaMap;

    // Generation parameters
    float frequency;
    float threshold;
    float persistence;
    int octaves;

    // Map dimensions
    int width;
    int height;

    // Random generator
    std::mt19937 rng;

public:
    // Constructor with default parameters
    LavaGenerator(int mapWidth, int mapHeight, unsigned int seed = std::random_device{}());

    // Generate the lava map
    void generate();

    // Check if a position contains lava
    bool isLava(int x, int y) const;

    // Set generation parameters
    void setFrequency(float freq) { frequency = freq; }
    void setThreshold(float thresh) { threshold = thresh; }
    void setPersistence(float persist) { persistence = persist; }
    void setOctaves(int oct) { octaves = oct; }

    // Get generation parameters
    float getFrequency() const { return frequency; }
    float getThreshold() const { return threshold; }
    float getPersistence() const { return persistence; }
    int getOctaves() const { return octaves; }

    // Get a debug visualization
    sf::Image getDebugImage() const;
};

#endif // LAVA_GENERATOR_H
