#include "chunk.h"
#include "constants.h"
#include <iostream>
#include <cmath>
#include <filesystem>

// Static members
std::map<std::string, sf::Texture> Chunk::textures;
bool Chunk::texturesLoaded = false;

Chunk::Chunk() {
    if (!texturesLoaded) {
        loadTextures();
        texturesLoaded = true;
    }

    // Initialize height and lava maps
    for (int x = 0; x < Chunks::size; x++) {
        for (int y = 0; y < Chunks::size; y++) {
            heightMap[x][y] = 0;
            lavaMap[x][y] = false;
        }
    }
}

void Chunk::loadTextures() {
    std::vector<std::pair<std::string, std::string>> files = {
        {"water", "../sprites/water-sprite.png"},
        {"grass", "../sprites/grassy-sprite.png"},
        {"dirt", "../sprites/rockydesert-sprite.png"},
        {"stone", "../sprites/stoney-sprite.png"},
        {"lava", "../sprites/lava-1.png"}
    };

    // Try different paths for each texture
    std::vector<std::string> basePaths = {
        "",
        "../",
        "./",
        "../../",
        "../../../"
    };

    for (auto& [key, relativePath] : files) {
        bool loaded = false;
        sf::Texture tex;

        for (const auto& basePath : basePaths) {
            std::string fullPath = basePath + relativePath;
            std::string spritesPath = basePath + "sprites/" + std::filesystem::path(relativePath).filename().string();

            std::vector<std::string> attemptPaths = {
                fullPath,
                spritesPath,
                // Add more variations if needed
            };

            for (const auto& path : attemptPaths) {
                std::cout << "Trying to load " << key << " texture from: " << path << std::endl;
                if (std::filesystem::exists(path) && tex.loadFromFile(path)) {
                    std::cout << "Successfully loaded " << key << " texture from: " << path << std::endl;
                    tex.setSmooth(true);
                    textures[key] = tex;
                    loaded = true;
                    break;
                }
            }

            if (loaded) break;
        }

        if (!loaded) {
            std::cerr << "Failed to load texture for: " << key << " from any path variations of: " << relativePath << std::endl;
        }
    }

    // Special handling for lava - check if it's in the spritesheet
    if (textures.find("lava") == textures.end()) {
        std::cout << "Lava texture not found separately. It might be included in the spritesheet." << std::endl;
    }
}

void Chunk::generateLava(sf::Vector2i coords, const siv::PerlinNoise &heightNoise,
                          const siv::PerlinNoise &lavaNoise, int octaves,
                          float persistence, float heightFreq, float lavaFreq) {
    float offsetX = 10000.0f;
    float offsetY = 5000.0f;

    for (int x = 0; x < Chunks::size; ++x) {
        for (int y = 0; y < Chunks::size; ++y) {
            float wx = static_cast<float>(coords.x + x) + offsetX;
            float wy = static_cast<float>(coords.y + y) + offsetY;

            // Generate height
            double baseNoise = heightNoise.normalizedOctave2D_01(wx * heightFreq, wy * heightFreq, octaves, persistence);
            double detailNoise = heightNoise.normalizedOctave2D_01(wx * heightFreq * 4.0f, wy * heightFreq * 4.0f, 2, 0.5);
            float curvedNoise = std::pow(static_cast<float>(baseNoise), 1.3f);
            curvedNoise += static_cast<float>((detailNoise - 0.5) * 0.1f);
            curvedNoise = std::clamp(curvedNoise, 0.05f, 1.0f);
            float finalNoise = 0.25f + 0.75f * curvedNoise;
            int height = static_cast<int>(20.0f * finalNoise); // Using 20 as default height scale
            height = std::clamp(height, 1, Chunks::maxHeight - 1);

            // Store the height
            this->heightMap[x][y] = height;

            // Generate lava
            double lavaNoiseSample = lavaNoise.normalizedOctave2D_01(wx * lavaFreq, wy * lavaFreq, 5, 0.6);
            this->lavaMap[x][y] = (lavaNoiseSample > 0.55f);
        }
    }

    // Count and report lava tiles in this chunk
    int lavaCount = 0;
    for (int x = 0; x < Chunks::size; ++x) {
        for (int y = 0; y < Chunks::size; ++y) {
            if (this->lavaMap[x][y]) lavaCount++;
        }
    }
}

bool Chunk::isLava(int x, int y) const {
    if (x < 0 || y < 0 || x >= Chunks::size || y >= Chunks::size) {
        return false;
    }
    return lavaMap[x][y];
}

int Chunk::getHeight(int x, int y) const {
    if (x < 0 || y < 0 || x >= Chunks::size || y >= Chunks::size) {
        return 0;
    }
    return heightMap[x][y];
}