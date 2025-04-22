#include "chunkManager.h"
#include "constants.h"
#include <iostream>

ChunkManager::ChunkManager(const siv::PerlinNoise &heightMap, const siv::PerlinNoise &lavaMap,
                         int octaves, float persistence, float frequency, float lavaFrequency)
    : heightMap(heightMap), lavaMap(lavaMap), octaves(octaves),
      persistence(persistence), frequency(frequency), lavaFrequency(lavaFrequency) {

    std::cout << "Initializing ChunkManager with lava frequency: " << lavaFrequency << std::endl;
    // Generate initial chunks
    generateChunks();
}

void ChunkManager::updateParameters(int newOctaves, float newPersistence, float newFrequency, float newLavaFrequency) {
    if (newOctaves != octaves || newPersistence != persistence ||
        newFrequency != frequency || newLavaFrequency != lavaFrequency) {

        std::cout << "Updating ChunkManager parameters:" << std::endl;
        std::cout << "  Octaves: " << octaves << " -> " << newOctaves << std::endl;
        std::cout << "  Persistence: " << persistence << " -> " << newPersistence << std::endl;
        std::cout << "  Frequency: " << frequency << " -> " << newFrequency << std::endl;
        std::cout << "  Lava Frequency: " << lavaFrequency << " -> " << newLavaFrequency << std::endl;

        octaves = newOctaves;
        persistence = newPersistence;
        frequency = newFrequency;
        lavaFrequency = newLavaFrequency;

        // Regenerate chunks with new parameters
        chunks.clear();
        globalLavaMap.clear();
        generateChunks();
    }
}

void ChunkManager::generateChunks() {
    std::cout << "Generating chunks for lava..." << std::endl;

    // Generate chunks in render distance
    for (int i = 0; i < Manage::renderDistance; ++i) {
        for (int j = 0; j < Manage::renderDistance; ++j) {
            loadChunk(i, j);
        }
    }

    // Update the global lava map
    updateGlobalLavaMap();

    // Count total lava tiles
    int totalLavaTiles = 0;
    for (const auto& [pos, isLava] : globalLavaMap) {
        if (isLava) totalLavaTiles++;
    }

    std::cout << "Generated " << chunks.size() << " chunks with a total of "
              << totalLavaTiles << " lava tiles" << std::endl;

    // If we have very few or no lava tiles, increase frequency and regenerate
    if (totalLavaTiles < 100 && lavaFrequency < 0.2f) {
        std::cout << "Too few lava tiles generated. Increasing lava frequency and regenerating..." << std::endl;
        float newLavaFreq = lavaFrequency * 1.5f;
        updateParameters(octaves, persistence, frequency, newLavaFreq);
    }
}

void ChunkManager::loadChunk(int chunkX, int chunkY) {
    if (chunkX < 0 || chunkY < 0 ||
        chunkX * Chunks::size >= Chunks::gridWidth ||
        chunkY * Chunks::size >= Chunks::gridHeight) {
        return;
    }

    sf::Vector2i chunkCoords(chunkX * Chunks::size, chunkY * Chunks::size);

    Chunk chunk;
    chunk.generateLava(chunkCoords, heightMap, lavaMap, octaves, persistence, frequency, lavaFrequency);
    chunks[{chunkX, chunkY}] = chunk;
}

void ChunkManager::updateGlobalLavaMap() {
    globalLavaMap.clear();

    for (auto& [coords, chunk] : chunks) {
        int baseX = coords.first * Chunks::size;
        int baseY = coords.second * Chunks::size;

        for (int x = 0; x < Chunks::size; ++x) {
            for (int y = 0; y < Chunks::size; ++y) {
                int worldX = baseX + x;
                int worldY = baseY + y;

                if (chunk.isLava(x, y)) {
                    globalLavaMap[sf::Vector2i(worldX, worldY)] = true;
                }
            }
        }
    }

    // Debug output
    std::cout << "Updated global lava map with " << globalLavaMap.size() << " entries" << std::endl;
}

bool ChunkManager::isLava(int worldX, int worldY) const {
    // Check global lava map first (faster)
    auto it = globalLavaMap.find(sf::Vector2i(worldX, worldY));
    if (it != globalLavaMap.end()) {
        return it->second;
    }

    // If not in global map, check the appropriate chunk
    auto [chunkX, chunkY] = worldToChunk(worldX, worldY);
    auto [localX, localY] = worldToLocal(worldX, worldY);

    auto chunkIt = chunks.find({chunkX, chunkY});
    if (chunkIt != chunks.end()) {
        return chunkIt->second.isLava(localX, localY);
    }

    return false;
}

bool ChunkManager::isWalkable(int worldX, int worldY) const {
    // If it's lava, it's not walkable
    if (isLava(worldX, worldY)) {
        return false;
    }

    // Check if it's in bounds
    if (worldX < 0 || worldY < 0 ||
        worldX >= Chunks::gridWidth ||
        worldY >= Chunks::gridHeight) {
        return false;
    }

    // If it's not lava and it's in bounds, it's walkable
    return true;
}

std::pair<int, int> ChunkManager::worldToChunk(int worldX, int worldY) const {
    return {worldX / Chunks::size, worldY / Chunks::size};
}

std::pair<int, int> ChunkManager::worldToLocal(int worldX, int worldY) const {
    return {worldX % Chunks::size, worldY % Chunks::size};
}