#include "LavaGenerator.h"
#include <iostream>

// Constructor
LavaGenerator::LavaGenerator(int mapWidth, int mapHeight, unsigned int seed) : width(mapWidth), height(mapHeight), rng(seed) {
    
    // Initialize Perlin noise with the seed
    perlin = siv::PerlinNoise(seed);
    
    // Default parameters - adjust these for different lava patterns
    frequency   = 0.035f;  // ↑ More clumps (smaller features)
    threshold   = 0.60f;   // ↑ Less lava overall (~20% coverage)
    persistence = 0.7f;    // ↑ Adds extra detail inside clumps
    octaves     = 5;       // Keep as-is (good balance)

}

// Generate the lava map
void LavaGenerator::generate() {
    std::cout << "Generating lava map..." << std::endl;
    lavaMap.clear();
    
    int lavaCount = 0;
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // Normalize coordinates for Perlin noise
            double nx = x * frequency;
            double ny = y * frequency;
            
            // Get noise value with fractal Brownian motion (multiple octaves)
            double value = perlin.normalizedOctave2D_01(nx, ny, octaves, persistence);
            
            // Add some variety with another noise layer
            double detailValue = perlin.normalizedOctave2D_01(nx * 2.5, ny * 2.5, 2, 0.5);
            value = value * 0.8 + detailValue * 0.2;
            
            // Apply threshold to determine if this is lava
            bool isLavaTile = (value > threshold);
            
            if (isLavaTile) {
                lavaMap[sf::Vector2i(x, y)] = true;
                lavaCount++;
            }
        }
    }
    
    // Post-process to create more interesting patterns
    // Add "lava veins" branching out from existing lava
    std::vector<sf::Vector2i> directions = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1}
    };
    
    auto lavaMapCopy = lavaMap;
    for (const auto& [pos, _] : lavaMapCopy) {
        // With 20% chance, extend lava in a random direction
        if (std::uniform_real_distribution<float>(0, 1)(rng) < 0.2f) {
            // Choose a random direction
            const sf::Vector2i& dir = directions[std::uniform_int_distribution<int>(0, 3)(rng)];
            sf::Vector2i newPos = pos + dir;
            
            // Check bounds
            if (newPos.x >= 0 && newPos.x < width && newPos.y >= 0 && newPos.y < height) {
                lavaMap[newPos] = true;
                lavaCount++;
            }
        }
    }
    
    // Add some small random lava pools
    int numPools = width * height / 500; // Adjust this ratio to control pool density
    for (int i = 0; i < numPools; ++i) {
        int poolX = std::uniform_int_distribution<int>(0, width - 1)(rng);
        int poolY = std::uniform_int_distribution<int>(0, height - 1)(rng);
        int poolSize = std::uniform_int_distribution<int>(3, 8)(rng);
        
        for (int y = -poolSize/2; y <= poolSize/2; ++y) {
            for (int x = -poolSize/2; x <= poolSize/2; ++x) {
                // Create a circular pool
                if (x*x + y*y <= poolSize*poolSize/4) {
                    int tileX = poolX + x;
                    int tileY = poolY + y;
                    
                    // Check bounds
                    if (tileX >= 0 && tileX < width && tileY >= 0 && tileY < height) {
                        // Only add the pool if it's near existing lava (30% chance)
                        if (std::uniform_real_distribution<float>(0, 1)(rng) < 0.3f) {
                            lavaMap[sf::Vector2i(tileX, tileY)] = true;
                            lavaCount++;
                        }
                    }
                }
            }
        }
    }
    
    std::cout << "Lava generation complete. Created " << lavaCount 
              << " lava tiles (" << (lavaCount * 100.0f / (width * height)) << "% coverage)" << std::endl;
}

// Check if a position contains lava
bool LavaGenerator::isLava(int x, int y) const {
    // Out of bounds is not lava
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return false;
    }
    
    sf::Vector2i pos(x, y);
    auto it = lavaMap.find(pos);
    
    // If position is in the map and marked as lava, return true
    return (it != lavaMap.end() && it->second);
}

// Generate a debug visualization
sf::Image LavaGenerator::getDebugImage() const {
    sf::Image img;
    img.create(width, height);
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (isLava(x, y)) {
                img.setPixel(x, y, sf::Color(255, 69, 0)); // Lava - orange-red
            } else {
                img.setPixel(x, y, sf::Color(34, 139, 34)); // Safe - forest green
            }
        }
    }
    
    return img;
}