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
#include <iostream>
#include <map>




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




    int tileWidth;
    int tileHeight;




public:
    Map();




    bool loadFromFile(const std::string& tmxFilePath, int tileWidth, int tileHeight);
    bool parseTilesets(tinyxml2::XMLElement* mapElement);
    bool parseLayers(tinyxml2::XMLElement* mapElement, int width, int height);
    void draw(sf::RenderWindow& window) const;




    int getWidth() const;
    int getHeight() const;
};




#endif // THE_FLOOR_IS_LAVA_MAP_H

