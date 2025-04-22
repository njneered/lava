#pragma once

#include <SFML/Graphics.hpp>
#include <map>
#include <vector>
#include "constants.h"
#include "perlin.hpp"

// Simplified Chunk class focused only on generating terrain and lava
class Chunk {
public:
  Chunk();

  // Generate a lava map based on noise
  void generateLava(sf::Vector2i coords, const siv::PerlinNoise &heightMap,
                    const siv::PerlinNoise &lavaMap, int octaves,
                    float persistence, float heightFreq, float lavaFreq);

  // Check if a specific position contains lava
  bool isLava(int x, int y) const;

  // Get the height at a specific position
  int getHeight(int x, int y) const;

private:
  static std::map<std::string, sf::Texture> textures;
  static bool texturesLoaded;
  static void loadTextures();

  int heightMap[Chunks::size][Chunks::size];
  bool lavaMap[Chunks::size][Chunks::size];
};