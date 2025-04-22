//
// Created by Nj on 4/5/2025.
//

#ifndef THE_FLOOR_IS_LAVA_MAP_H
#define THE_FLOOR_IS_LAVA_MAP_H

#pragma once
#include <SFML/Graphics.hpp>
#include <tinyxml2-master/tinyxml2.h>
#include <vector>
#include <string>
#include <map>
using namespace std;


class Map {
    struct TileLayer {
        string name;
        vector<unsigned> tileIDs;
        int width;
        int height;
    };

    vector<TileLayer> layers;
    map<unsigned, sf::Texture> tileTextures;
    map<unsigned, sf::Sprite> tileSprites;
    vector<sf::Vector2i> lavaTiles;
    sf::Texture lavaTexture;
    sf::Sprite lavaSprite;

    unsigned tilesetFirstGID;
    int tileWidth;
    int tileHeight;

    float lavaTimer = 0.0f;

public:
    Map();

    bool loadFromFile(const std::string& tmxFilePath, int tileWidth, int tileHeight);
    bool parseTilesets(tinyxml2::XMLElement* mapElement);
    bool parseLayers(tinyxml2::XMLElement* mapElement, int width, int height);
    void generateSpreadingLava(int seedCount, int initialLavaPerSeed);
    bool isLava(int x, int y) const;
    const std::vector<sf::Vector2i>& getLavaTiles() const;

    void draw(sf::RenderWindow& window) const;
    void updateLavaSpread(float deltaTime);

    int getWidth() const;
    int getHeight() const;
};




#endif // THE_FLOOR_IS_LAVA_MAP_H

