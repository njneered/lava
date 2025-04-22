#ifndef CHUNKMANAGER_H
#define CHUNKMANAGER_H

#include <mutex>
#include <map>
#include <unordered_map>

#include "chunk.h"
#include "utility.h"

// Simplified ChunkManager that focuses on lava generation and tracking
class ChunkManager {
public:
  ChunkManager(const siv::PerlinNoise &heightMap, const siv::PerlinNoise &lavaMap,
               int octaves, float persistence, float frequency, float lavaFrequency);

  // Update noise parameters
  void updateParameters(int newOctaves, float newPersistence, float newFrequency, float newLavaFrequency);

  // Generate chunks and update lava map
  void generateChunks();

  // Check if a specific world position contains lava
  bool isLava(int worldX, int worldY) const;

  // Get walkability at a specific position (opposite of lava)
  bool isWalkable(int worldX, int worldY) const;

private:
  const siv::PerlinNoise &heightMap;
  const siv::PerlinNoise &lavaMap;

  std::map<std::pair<int, int>, Chunk> chunks;
  std::unordered_map<sf::Vector2i, bool, Vector2iHash> globalLavaMap;

  int octaves;
  float persistence;
  float frequency;
  float lavaFrequency;

  // Load a specific chunk
  void loadChunk(int chunkX, int chunkY);

  // Update the global lava map
  void updateGlobalLavaMap();

  // Convert world coordinates to chunk coordinates
  std::pair<int, int> worldToChunk(int worldX, int worldY) const;

  // Convert world coordinates to local chunk coordinates
  std::pair<int, int> worldToLocal(int worldX, int worldY) const;
};

#endif