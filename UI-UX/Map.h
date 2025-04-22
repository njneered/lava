#ifndef THE_FLOOR_IS_LAVA_MAP_H
#define THE_FLOOR_IS_LAVA_MAP_H

#pragma once
#include <SFML/Graphics.hpp>
#include <tinyxml2-master/tinyxml2.h>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include "LavaGenerator.h" // Add the LavaGenerator

class Map {
    struct TileLayer {
        std::string name;
        std::vector<unsigned> tileIDs;
        int width;
        int height;
    };

    std::vector<TileLayer> layers;
    std::map<unsigned, sf::Texture> tileTextures;
    std::map<unsigned, sf::Sprite> tileSprites;

    unsigned tilesetFirstGID;
    int tileWidth;
    int tileHeight;
    
    // Lava generator
    LavaGenerator* lavaGenerator;
    bool useLavaGenerator;
    
    // Walkability map - used for pathfinding
    std::unordered_map<sf::Vector2i, bool, Vector2iHash> walkableMap;

    sf::Texture lavaTexture;

public:
    Map();
    ~Map();

    bool loadFromFile(const std::string& tmxFilePath, int tileWidth, int tileHeight);
    bool parseTilesets(tinyxml2::XMLElement* mapElement);
    bool parseLayers(tinyxml2::XMLElement* mapElement, int width, int height);
    
    // Initialize lava generator
    void initLavaGenerator(unsigned int seed = std::random_device{}());
    
    // Generate lava
    void generateLava();
    
    // Set lava generation parameters
    void setLavaFrequency(float freq);
    void setLavaThreshold(float thresh);
    void setLavaPersistence(float persist);
    void setLavaOctaves(int oct);

    void draw(sf::RenderWindow& window) const;
    
    // Check if a position is walkable (not lava)
    bool isWalkable(int x, int y) const;
    
    // Check if a position contains lava
    bool isLava(int x, int y) const;

    int getWidth() const;
    int getHeight() const;
    
    // Convert between screen and tile coordinates
    sf::Vector2f tileToScreen(int x, int y) const;
    sf::Vector2i screenToTile(float x, float y) const;
};

#endif // THE_FLOOR_IS_LAVA_MAP_H