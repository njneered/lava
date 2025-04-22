#include <random>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <filesystem>

#include "UI-UX/Background.h"
#include "UI-UX/NaviGator.h"
#include "UI-UX/Title.h"
#include "UI-UX/Soundtrack.h"
#include "UI-UX/UX.h"
#include "UI-UX/GPS.h"
#include "../terrain/chunkManager.h" // Now only used for lava generation
#include "../UI-UX/map.h" // Added map for general terrain
#include "algorithms/Algorithms.h"

namespace fs = std::filesystem;

bool verifyPathSegments(const std::vector<sf::Vector2i>& path, const Map& map) {
    if (path.size() <= 1) return true; // Nothing to verify

    for (size_t i = 0; i < path.size() - 1; ++i) {
        sf::Vector2i current = path[i];
        sf::Vector2i next = path[i+1];

        // Calculate Manhattan distance
        int dx = std::abs(next.x - current.x);
        int dy = std::abs(next.y - current.y);

        // If not adjacent, this could be a problem
        if (dx + dy > 1) {
            std::cout << "[Verify] WARNING: Non-adjacent tiles in path: ("
                      << current.x << "," << current.y << ") to ("
                      << next.x << "," << next.y << ")\n";
            return false;
        }

        // Verify both tiles are walkable
        if (!map.isWalkable(current.x, current.y)) {
            std::cout << "[Verify] ERROR: Path contains unwalkable tile at ("
                      << current.x << "," << current.y << ")\n";
            return false;
        }

        if (!map.isWalkable(next.x, next.y)) {
            std::cout << "[Verify] ERROR: Path contains unwalkable tile at ("
                      << next.x << "," << next.y << ")\n";
            return false;
        }
    }

    std::cout << "[Verify] Path contains only adjacent walkable tiles\n";
    return true;
}

int main() {
    // Initialize window
    sf::RenderWindow window(sf::VideoMode(1280, 720), "The Floor is Lava!!!");
    window.setFramerateLimit(60);

    // === Title Screen ===
    Background background("assets/sky.jpeg");
    Title title;
    while (window.isOpen() && !title.isFinished()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            title.handleEvent(event);
        }
        window.clear();
        window.setView(window.getDefaultView());
        background.draw(window);
        title.draw(window);
        window.display();
    }

    // === View & Zoom Setup ===
    sf::View view;
    float zoomLevel = 0.5f;
    const float minZoom = 0.1f;
    const float maxZoom = 2.5f;

    const int gridWidth = Chunks::gridWidth;
    const int gridHeight = Chunks::gridHeight;
    const float tileWidth = 32.f;
    const float tileHeight = 16.f;

    float mapPixelWidth = (gridWidth + gridHeight) * (tileWidth / 2.f);
    float mapPixelHeight = (gridWidth + gridHeight) * (tileHeight / 2.f);
    sf::Vector2f mapCenter(mapPixelWidth / 2.f, mapPixelHeight / 2.f);

    // Initial view setup
    view.setSize(mapPixelWidth * zoomLevel, mapPixelHeight * zoomLevel);
    view.setCenter(mapCenter);

    // === Starting Position and Navigator Setup ===
    sf::Vector2f startingPos(650.f, 600.f);
    NaviGator navigator("sprites/navigator.png", startingPos);
    navigator.setPosition(startingPos);

    // === Noise and Terrain Setup ===
    std::cout << "=== INITIALIZING TERRAIN GENERATION ===\n";

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(100000, 999999);
    int heightSeed = dis(gen), biomeSeed = dis(gen), lavaSeed = dis(gen);

    std::cout << "Seeds: Height=" << heightSeed << ", Biome=" << biomeSeed << ", Lava=" << lavaSeed << std::endl;

    const siv::PerlinNoise heightMap{ static_cast<siv::PerlinNoise::seed_type>(heightSeed) };
    const siv::PerlinNoise biomeMap{ static_cast<siv::PerlinNoise::seed_type>(biomeSeed) };
    const siv::PerlinNoise lavaMap{ static_cast<siv::PerlinNoise::seed_type>(lavaSeed) };

    // Try different lava frequencies if needed
    float frequency = 0.01f, lavaFrequency = 0.05f, persistence = 0.3f;
    int octaves = 8;

    // Create ChunkManager for lava generation with slightly higher lava frequency
    ChunkManager chunkManager(heightMap, lavaMap, octaves, persistence, frequency, lavaFrequency);

    // Initialize lava map
    chunkManager.generateChunks();

    // Create and setup Map
    Map gameMap;

    // Check working directory
    std::cout << "Current working directory: " << fs::current_path() << std::endl;

    // Try to load TMX file from multiple possible locations
    std::vector<std::string> possibleMapPaths = {
        "maps/floorIsLava.tmx"
    };

    bool mapLoaded = false;
    for (const auto& mapPath : possibleMapPaths) {
        std::cout << "Attempting to load map from: " << mapPath << std::endl;
        if (fs::exists(mapPath) && gameMap.loadFromFile(mapPath, tileWidth, tileHeight)) {
            std::cout << "Successfully loaded map from: " << mapPath << std::endl;
            mapLoaded = true;
            break;
        }
    }

    // If we couldn't load the TMX file, let's try to create a custom map
    if (!mapLoaded) {
        std::cout << "No TMX file found. Creating a fallback map..." << std::endl;

        // We'll use the procedurally generated lava information from the ChunkManager
        // You might need to add a method to create a basic map with just lava data

        // Link the ChunkManager to the Map for lava detection
        gameMap.setChunkManager(&chunkManager);

        std::cout << "Using procedural lava generation only." << std::endl;
    } else {
        // If map was loaded, still link the ChunkManager for lava
        gameMap.setChunkManager(&chunkManager);
    }

    // Debug: check for lava around the starting area
    float ax = (startingPos.y / 8.0f + startingPos.x / 16.0f) / 2.0f;
    float ay = (startingPos.y / 8.0f - startingPos.x / 16.0f) / 2.0f;
    sf::Vector2i startTile(static_cast<int>(std::round(ax)), static_cast<int>(std::round(ay)));

    // Check if starting position is on lava
    if (chunkManager.isLava(startTile.x, startTile.y)) {
        std::cout << "Warning: Starting position is on lava!" << std::endl;
        // Move the start position to avoid lava
        bool foundSafeTile = false;
        for (int radius = 1; radius < 10 && !foundSafeTile; radius++) {
            for (int dx = -radius; dx <= radius && !foundSafeTile; dx++) {
                for (int dy = -radius; dy <= radius && !foundSafeTile; dy++) {
                    int newX = startTile.x + dx;
                    int newY = startTile.y + dy;
                    if (newX >= 0 && newY >= 0 &&
                        newX < Chunks::gridWidth && newY < Chunks::gridHeight &&
                        !chunkManager.isLava(newX, newY)) {

                        // Convert back to screen coords
                        sf::Vector2f screenPos = gameMap.tileToScreen(newX, newY);
                        navigator.setPosition(screenPos);
                        startingPos = screenPos;
                        std::cout << "Moved start position to safe tile at: ("
                                  << newX << "," << newY << ")" << std::endl;
                        foundSafeTile = true;
                    }
                }
            }
        }
    }

    // === UI Setup ===
    Background gameBackground("assets/sky.jpeg");
    sf::Clock clock;
    UX ux("The Floor is Lava!\nUse WASD to move.");
    ux.setSecondaryMessage("Click the box below and type a destination tile (e.g. 65 9)");
    ux.setInstructionMessage("Then press Enter to confirm.");

    sf::Vector2i destinationPoint;
    GPS gps;
    bool coordinatesReady = false;

    // === Main Game Loop ===
    while (window.isOpen()) {
        sf::Time frameTime = clock.restart();
        sf::Event event;

        // === Event Handling ===
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // Handle UI events
            ux.handleEvent(event, window);

            // Handle zoom
            if (event.type == sf::Event::MouseWheelScrolled) {
                if (event.mouseWheelScroll.delta > 0 && zoomLevel > minZoom) {
                    zoomLevel *= 0.9f;
                } else if (event.mouseWheelScroll.delta < 0 && zoomLevel < maxZoom) {
                    zoomLevel *= 1.1f;
                }
                view.setSize(mapPixelWidth * zoomLevel, mapPixelHeight * zoomLevel);
            }

            // Handle coordinate input
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter && ux.hasValidInput()) {
                destinationPoint = ux.getTypedCoordinates();
                destinationPoint.x = std::clamp(destinationPoint.x, 0, Chunks::gridWidth - 1);
                destinationPoint.y = std::clamp(destinationPoint.y, 0, Chunks::gridHeight - 1);

                // Check if destination is valid (not on lava)
                if (!gameMap.isWalkable(destinationPoint.x, destinationPoint.y)) {
                    ux.setConfirmationMessage("Destination is on lava! Please choose another location.");
                    coordinatesReady = false;
                } else {
                    std::stringstream ss;
                    ss << "Destination set to (" << destinationPoint.x << ", " << destinationPoint.y << ")\n";
                    ss << "Press 1 for Dijkstra, 2 for A*, 3 to compare both";
                    ux.setConfirmationMessage(ss.str());
                    coordinatesReady = true;
                }

                ux.resetTypedCoordinates();
            }

            // Generate paths with number keys
            if (coordinatesReady && event.type == sf::Event::KeyPressed && !ux.isTypingActive()) {
                if (event.key.code == sf::Keyboard::Num1 ||
                    event.key.code == sf::Keyboard::Num2 ||
                    event.key.code == sf::Keyboard::Num3) {

                    // Convert navigator's isometric screen position to tile coordinates
                    float ax = (navigator.getPosition().y / 8.0f + navigator.getPosition().x / 16.0f) / 2.0f;
                    float ay = (navigator.getPosition().y / 8.0f - navigator.getPosition().x / 16.0f) / 2.0f;
                    sf::Vector2i startPoint(static_cast<int>(std::round(ax)), static_cast<int>(std::round(ay)));
                    startPoint.x = std::clamp(startPoint.x, 0, Chunks::gridWidth - 1);
                    startPoint.y = std::clamp(startPoint.y, 0, Chunks::gridHeight - 1);

                    // Check if starting point is valid
                    if (!gameMap.isWalkable(startPoint.x, startPoint.y)) {
                        ux.setConfirmationMessage("You're on lava! Move to a safe tile first.");
                        continue;
                    }

                    // Run appropriate pathfinding algorithm(s)
                    if (event.key.code == sf::Keyboard::Num1) {
                        auto path = findDijkstraPath(gameMap, startPoint, destinationPoint);
                        if (path.empty()) {
                            ux.setConfirmationMessage("Dijkstra could not find a path! Try another destination.");
                        } else {
                            // Verify path segments
                            if (!verifyPathSegments(path, gameMap)) {
                                std::cout << "[Main] Path contains invalid segments!\n";
                                ux.setConfirmationMessage("Warning: Path contains non-adjacent segments!");
                            }

                            gps.setPath(path, DIJKSTRA);

                            std::stringstream ss;
                            ss << "Dijkstra path found with " << path.size() << " steps";
                            ux.setConfirmationMessage(ss.str());
                        }
                    } else if (event.key.code == sf::Keyboard::Num2) {
                        auto path = findAStarPath(gameMap, startPoint, destinationPoint);
                        if (path.empty()) {
                            ux.setConfirmationMessage("A* could not find a path! Try another destination.");
                        } else {
                            // Verify path segments
                            if (!verifyPathSegments(path, gameMap)) {
                                std::cout << "[Main] Path contains invalid segments!\n";
                                ux.setConfirmationMessage("Warning: Path contains non-adjacent segments!");
                            }

                            gps.setPath(path, ASTAR);

                            std::stringstream ss;
                            ss << "A* path found with " << path.size() << " steps";
                            ux.setConfirmationMessage(ss.str());
                        }
                    } else if (event.key.code == sf::Keyboard::Num3) {
                        auto pathD = findDijkstraPath(gameMap, startPoint, destinationPoint);
                        auto pathA = findAStarPath(gameMap, startPoint, destinationPoint);

                        if (pathD.empty() && pathA.empty()) {
                            ux.setConfirmationMessage("Neither algorithm could find a path! Try another destination.");
                        } else {
                            // Verify both paths
                            bool dijkstraValid = pathD.empty() ? true : verifyPathSegments(pathD, gameMap);
                            bool astarValid = pathA.empty() ? true : verifyPathSegments(pathA, gameMap);

                            if (!dijkstraValid || !astarValid) {
                                std::cout << "[Main] One or both paths contain invalid segments!\n";
                            }

                            gps.setComparisonPaths(pathD, pathA);

                            // Show comparison info
                            std::stringstream ss;
                            ss << "Dijkstra: " << pathD.size() << " steps, A*: " << pathA.size() << " steps";
                            if (pathD.empty()) ss << " (Dijkstra failed)";
                            if (pathA.empty()) ss << " (A* failed)";
                            if (!dijkstraValid) ss << " (Dijkstra has invalid segments)";
                            if (!astarValid) ss << " (A* has invalid segments)";
                            ux.setConfirmationMessage(ss.str());
                        }
                    }
                }
            }
        }

        navigator.handleInput(frameTime);
        navigator.update(frameTime);
        gps.update(frameTime.asSeconds());

        view.setCenter(navigator.getPosition());
        window.setView(view);

        window.clear(sf::Color::Black);

        window.setView(window.getDefaultView());
        gameBackground.draw(window);

        window.setView(view);

        // Draw the map
        gameMap.draw(window);

        // Draw the GPS paths
        gps.draw(window, gameMap);

        // Draw the navigator
        navigator.draw(window);

        window.setView(window.getDefaultView());
        ux.updateCursor();
        ux.draw(window);

        window.display();
    }

    return 0;
}