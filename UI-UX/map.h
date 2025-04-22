#ifndef MAP_H
#define MAP_H

#include <SFML/Graphics.hpp>
#include <tinyxml2-master/tinyxml2.h>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include "../terrain/utility.h"
#include "../terrain/chunkManager.h"

class Map {
private:
    struct TileLayer {
        std::string name;
        std::vector<unsigned> tileIDs;
        int width;
        int height;
    };

    std::vector<TileLayer> layers;
    std::map<unsigned, sf::Texture> tileTextures;

    unsigned tilesetFirstGID;
    int tileWidth;
    int tileHeight;

    // Reference to the chunk manager for lava detection
    ChunkManager* chunkManager;

    // Base walkability map (excludes lava which is handled by ChunkManager)
    std::unordered_map<sf::Vector2i, bool, Vector2iHash> walkableMap;

public:
    Map();

    // Set the chunk manager reference
    void setChunkManager(ChunkManager* cm);

    bool loadFromFile(const std::string& tmxFilePath, int tw, int th);
    bool parseTilesets(tinyxml2::XMLElement* mapElement);
    bool parseLayers(tinyxml2::XMLElement* mapElement, int width, int height);
    bool parseTileProperties(tinyxml2::XMLElement* mapElement);

    void draw(sf::RenderWindow& window);

    // Check if a tile is walkable, consulting both base walkability and lava from ChunkManager
    bool isWalkable(int x, int y) const;

    // Check if a tile contains lava (delegated to ChunkManager)
    bool isLava(int x, int y) const;

    int getWidth() const;
    int getHeight() const;

    sf::Vector2f tileToScreen(int x, int y) const;
    sf::Vector2i screenToTile(float x, float y) const;
};

#endif