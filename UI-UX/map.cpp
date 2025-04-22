#include "map.h"
#include <cmath>
#include <iostream>
#include <sstream>
#include <filesystem>  // Added for path checking

using namespace tinyxml2;
using namespace std;

Map::Map() : tileWidth(32), tileHeight(16), chunkManager(nullptr) {}

void Map::setChunkManager(ChunkManager* cm) {
    chunkManager = cm;
}

bool Map::loadFromFile(const std::string& tmxFilePath, int tw, int th) {
    tileWidth = tw;
    tileHeight = th;

    // Check if the file exists
    if (!std::filesystem::exists(tmxFilePath)) {
        cerr << "Error: TMX file does not exist at path: " << tmxFilePath << "\n";
        return false;
    }

    XMLDocument doc;
    if (doc.LoadFile(tmxFilePath.c_str()) != XML_SUCCESS) {
        cerr << "Failed to load TMX map file: " << tmxFilePath << "\n";
        cerr << "XML Error ID: " << doc.ErrorID() << "\n";
        cerr << "XML Error Name: " << doc.ErrorName() << "\n";
        return false;
    }

    XMLElement* mapElement = doc.FirstChildElement("map");
    if (mapElement == nullptr) {
        cerr << "No <map> element found.\n";
        return false;
    }

    int width = mapElement->IntAttribute("width");
    int height = mapElement->IntAttribute("height");

    cout << "Map dimensions: " << width << "x" << height << endl;

    if (!parseTilesets(mapElement)) {
        cerr << "Failed to parse tilesets\n";
        return false;
    }

    if (!parseLayers(mapElement, width, height)) {
        cerr << "Failed to parse layers\n";
        return false;
    }

    if (!parseTileProperties(mapElement)) {
        cerr << "Failed to parse tile properties\n";
        // We can continue even if properties fail to parse
    }

    cout << "Map loaded successfully!" << endl;
    return true;
}

bool Map::parseTilesets(XMLElement* mapElement) {
    for (XMLElement* tilesetElem = mapElement->FirstChildElement("tileset");
         tilesetElem != nullptr;
         tilesetElem = tilesetElem->NextSiblingElement("tileset")) {

        tilesetFirstGID = tilesetElem->IntAttribute("firstgid");

        cout << "Parsing tileset with firstgid: " << tilesetFirstGID << endl;

        // Check for embedded tileset first
        XMLElement* imageElem = tilesetElem->FirstChildElement("image");
        if (imageElem) {
            // Handle embedded tileset
            const char* imagePath = imageElem->Attribute("source");
            if (!imagePath) {
                cerr << "Image tag found but no 'source' attribute.\n";
                continue;
            }

            // Try different possible paths for the spritesheet
            vector<string> possiblePaths = {
                "sprites/" + string(imagePath),
                "../sprites/" + string(imagePath),
                string(imagePath),
                "./sprites/" + string(imagePath),
                "../../sprites/" + string(imagePath)
            };

            bool loaded = false;
            sf::Texture texture;

            for (const auto& path : possiblePaths) {
                cout << "Trying to load texture from: " << path << endl;
                if (std::filesystem::exists(path) && texture.loadFromFile(path)) {
                    cout << "Successfully loaded texture from: " << path << endl;
                    texture.setSmooth(true);
                    tileTextures[tilesetFirstGID] = texture;
                    loaded = true;
                    break;
                }
            }

            if (!loaded) {
                cerr << "Failed to load spritesheet for embedded tileset. Tried paths:" << endl;
                for (const auto& path : possiblePaths) {
                    cerr << "  - " << path << (std::filesystem::exists(path) ? " (exists)" : " (not found)") << endl;
                }
                // Continue anyway in case other tilesets load
            }
            continue;
        }

        // Check for external tileset
        const char* source = tilesetElem->Attribute("source");
        if (!source) {
            cerr << "No embedded image or external source found in tileset.\n";
            continue;
        }

        // Try different paths for the TSX file
        vector<string> possibleTsxPaths = {
            "maps/" + string(source),
            "../maps/" + string(source),
            string(source),
            "./maps/" + string(source),
            "../../maps/" + string(source)
        };

        bool tsxLoaded = false;
        XMLDocument tsxDoc;
        string loadedTsxPath;

        for (const auto& tsxPath : possibleTsxPaths) {
            cout << "Trying to load external tileset from: " << tsxPath << endl;
            if (std::filesystem::exists(tsxPath) && tsxDoc.LoadFile(tsxPath.c_str()) == XML_SUCCESS) {
                loadedTsxPath = tsxPath;
                tsxLoaded = true;
                break;
            }
        }

        if (!tsxLoaded) {
            cerr << "Failed to load external tileset. Tried paths:" << endl;
            for (const auto& path : possibleTsxPaths) {
                cerr << "  - " << path << (std::filesystem::exists(path) ? " (exists)" : " (not found)") << endl;
            }
            continue;
        }

        cout << "Successfully loaded external tileset from: " << loadedTsxPath << endl;

        XMLElement* tsxRoot = tsxDoc.FirstChildElement("tileset");
        if (!tsxRoot) {
            cerr << "No <tileset> found in .tsx file.\n";
            continue;
        }

        imageElem = tsxRoot->FirstChildElement("image");
        if (!imageElem) {
            cerr << "No image found in tileset: " << source << "\n";
            continue;
        }

        const char* imagePath = imageElem->Attribute("source");
        if (!imagePath) {
            cerr << "Image tag found but no 'source' attribute.\n";
            continue;
        }

        // Extract the directory from the loaded TSX path
        string tsxDir = loadedTsxPath.substr(0, loadedTsxPath.find_last_of("/\\") + 1);

        // Try different paths for the image
        vector<string> possibleImagePaths = {
            tsxDir + string(imagePath),
            "sprites/" + string(imagePath),
            "../sprites/" + string(imagePath),
            string(imagePath),
            "./sprites/" + string(imagePath)
        };

        bool imageLoaded = false;
        sf::Texture texture;

        for (const auto& path : possibleImagePaths) {
            cout << "Trying to load tileset image from: " << path << endl;
            if (std::filesystem::exists(path) && texture.loadFromFile(path)) {
                cout << "Successfully loaded tileset image from: " << path << endl;
                texture.setSmooth(true);
                tileTextures[tilesetFirstGID] = texture;
                imageLoaded = true;
                break;
            }
        }

        if (!imageLoaded) {
            cerr << "Failed to load tileset image. Tried paths:" << endl;
            for (const auto& path : possibleImagePaths) {
                cerr << "  - " << path << (std::filesystem::exists(path) ? " (exists)" : " (not found)") << endl;
            }
        }
    }

    if (tileTextures.empty()) {
        cerr << "No tilesets were loaded successfully!\n";
        return false;
    }

    cout << "Successfully loaded " << tileTextures.size() << " tilesets\n";
    return true;
}

bool Map::parseLayers(XMLElement* mapElement, int width, int height) {
    for (XMLElement* layerElem = mapElement->FirstChildElement("layer");
         layerElem != nullptr;
         layerElem = layerElem->NextSiblingElement("layer")) {

        bool visible = layerElem->BoolAttribute("visible", true);
        if (!visible) continue;

        const char* layerName = layerElem->Attribute("name");
        XMLElement* dataElem = layerElem->FirstChildElement("data");

        if (dataElem == nullptr || dataElem->GetText() == nullptr) {
            cerr << "Skipping layer with no data.\n";
            continue;
        }

        cout << "Parsing layer: " << (layerName ? layerName : "Unnamed") << endl;

        vector<unsigned> tileIDs;
        stringstream ss(dataElem->GetText());
        string token;

        while (getline(ss, token, ',')) {
            token.erase(0, token.find_first_not_of(" \n\r\t"));
            token.erase(token.find_last_not_of(" \n\r\t") + 1);

            if (!token.empty()) {
                try {
                    unsigned value = static_cast<unsigned>(stoi(token));
                    tileIDs.push_back(value);
                }
                catch (const invalid_argument& e) {
                    cerr << "Invalid tile ID: \"" << token << "\" (not a number)\n";
                }
                catch (const out_of_range& e) {
                    cerr << "Tile ID out of range: \"" << token << "\"\n";
                }
            }
        }

        TileLayer layer;
        layer.name = layerName ? layerName : "Unnamed";
        layer.tileIDs = tileIDs;
        layer.width = width;
        layer.height = height;

        cout << "Loaded layer \"" << layer.name << "\" with "
             << tileIDs.size() << " tiles (" << width << "x" << height
             << " = " << (width * height) << " expected)" << endl;

        layers.push_back(layer);

        // Initialize walkable map (assume all tiles are walkable by default)
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int index = x + y * width;

                // Skip out of bounds indices
                if (index >= static_cast<int>(tileIDs.size())) continue;

                unsigned tileID = tileIDs[index];

                // Skip empty tiles
                if (tileID == 0) continue;

                // Default: all tiles are walkable except water
                // Note: lava is now handled by ChunkManager
                sf::Vector2i tilePos(x, y);
                walkableMap[tilePos] = true;

                // Mark water tiles as unwalkable
                // This assumes that water is tileID 8-16
                if (tileID >= 8 && tileID <= 16) {
                    walkableMap[tilePos] = false;
                }
            }
        }
    }

    return !layers.empty();
}

bool Map::parseTileProperties(XMLElement* mapElement) {
    // Process tile properties from the tileset
    for (XMLElement* tilesetElem = mapElement->FirstChildElement("tileset");
         tilesetElem != nullptr;
         tilesetElem = tilesetElem->NextSiblingElement("tileset")) {

        unsigned firstGid = tilesetElem->IntAttribute("firstgid");

        // Check for tiles with specific properties
        for (XMLElement* tileElem = tilesetElem->FirstChildElement("tile");
             tileElem != nullptr;
             tileElem = tileElem->NextSiblingElement("tile")) {

            int tileId = tileElem->IntAttribute("id") + firstGid;

            XMLElement* propertiesElem = tileElem->FirstChildElement("properties");
            if (!propertiesElem) continue;

            for (XMLElement* propertyElem = propertiesElem->FirstChildElement("property");
                 propertyElem != nullptr;
                 propertyElem = propertyElem->NextSiblingElement("property")) {

                const char* name = propertyElem->Attribute("name");
                const char* value = propertyElem->Attribute("value");

                if (!name || !value) continue;

                // Process "walkable" property
                if (string(name) == "walkable") {
                    bool isWalkable = (string(value) != "false");

                    // Find all tiles with this ID and update their walkable status
                    for (const auto& layer : layers) {
                        for (int y = 0; y < layer.height; ++y) {
                            for (int x = 0; x < layer.width; ++x) {
                                int index = x + y * layer.width;
                                if (index >= static_cast<int>(layer.tileIDs.size())) continue;

                                if (layer.tileIDs[index] == tileId) {
                                    walkableMap[sf::Vector2i(x, y)] = isWalkable;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return true;
}

void Map::draw(sf::RenderWindow& window) {
    if (tileTextures.empty() || layers.empty()) {
        std::cerr << "Cannot draw map: No textures or no layers loaded!" << std::endl;
        return;
    }

    auto it = tileTextures.begin();
    const sf::Texture& tilesheet = it->second;

    // These values should match your spritesheet
    const int margin = 1;
    const int spacing = 2;
    const int columns = 6;
    const int actualTileWidth = 32;
    const int actualTileHeight = 64;
    const int logicalTileWidth = tileWidth;
    const int logicalTileHeight = tileHeight;

    sf::Sprite sprite;
    sprite.setTexture(tilesheet);

    // Force a wide rendering area
    int width = layers[0].width;
    int height = layers[0].height;

    // Calculate visible area with enormous padding to ensure full view
    sf::View view = window.getView();
    sf::Vector2f viewCenter = view.getCenter();

    // Convert center position to tile coordinates
    sf::Vector2i centerTile = screenToTile(viewCenter.x, viewCenter.y);

    // Define a large viewing area around the center
    const int VIEW_RADIUS = 100; // Large radius
    sf::Vector2i topLeft(centerTile.x - VIEW_RADIUS, centerTile.y - VIEW_RADIUS);
    sf::Vector2i bottomRight(centerTile.x + VIEW_RADIUS, centerTile.y + VIEW_RADIUS);

    // Clamp to map bounds
    topLeft.x = std::max(0, topLeft.x);
    topLeft.y = std::max(0, topLeft.y);
    bottomRight.x = std::min(width - 1, bottomRight.x);
    bottomRight.y = std::min(height - 1, bottomRight.y);

    std::cout << "Drawing area: (" << topLeft.x << "," << topLeft.y << ") to ("
              << bottomRight.x << "," << bottomRight.y << ")" << std::endl;

    // Draw ground layer
    const TileLayer& groundLayer = layers[0];

    // Draw from left to right, top to bottom
    for (int y = topLeft.y; y <= bottomRight.y; ++y) {
        for (int x = topLeft.x; x <= bottomRight.x; ++x) {
            int index = x + y * width;
            if (index >= static_cast<int>(groundLayer.tileIDs.size()) || index < 0)
                continue;

            unsigned tileID = groundLayer.tileIDs[index];

            // If there's lava at this position, use lava tile instead
            if (chunkManager && chunkManager->isLava(x, y)) {
                tileID = 5; // Use tile #5 for lava (adjust if needed)
            }

            // Skip empty tiles
            if (tileID == 0)
                continue;

            unsigned gid = tileID - tilesetFirstGID;

            // Check bounds
            if (gid >= columns * (tilesheet.getSize().y / (actualTileHeight + spacing)))
                continue;

            int tx = gid % columns;
            int ty = gid / columns;

            int textureX = margin + tx * (actualTileWidth + spacing);
            int textureY = margin + ty * (actualTileHeight + spacing);

            // More bounds checking
            if (textureX + actualTileWidth > tilesheet.getSize().x ||
                textureY + actualTileHeight > tilesheet.getSize().y)
                continue;

            sprite.setTextureRect(sf::IntRect(textureX, textureY,
                                actualTileWidth, actualTileHeight));

            float isoX = static_cast<float>((x - y) * (logicalTileWidth / 2.0f));
            float isoY = static_cast<float>((x + y) * (logicalTileHeight / 2.0f));
            isoY -= (actualTileHeight - logicalTileHeight);

            sprite.setPosition(isoX, isoY);
            window.draw(sprite);
        }
    }
}

// Rest of the methods remain unchanged
bool Map::isWalkable(int x, int y) const {
    sf::Vector2i pos(x, y);

    // Check if out of bounds
    if (layers.empty() || x < 0 || y < 0 || x >= Chunks::gridWidth || y >= Chunks::gridHeight) {
        return false;
    }

    // Check with ChunkManager first for lava
    if (chunkManager && chunkManager->isLava(x, y)) {
        return false;
    }

    // Then check our base walkability map
    auto it = walkableMap.find(pos);
    if (it != walkableMap.end()) {
        return it->second;
    }

    // Default to walkable if not specified
    return true;
}

bool Map::isLava(int x, int y) const {
    // Delegate to ChunkManager
    return chunkManager ? chunkManager->isLava(x, y) : false;
}

int Map::getWidth() const {
    return Chunks::gridWidth;
}

int Map::getHeight() const {
    return Chunks::gridHeight;
}

sf::Vector2f Map::tileToScreen(int x, int y) const {
    float isoX = static_cast<float>((x - y) * (tileWidth / 2.0f));
    float isoY = static_cast<float>((x + y) * (tileHeight / 2.0f));
    return sf::Vector2f(isoX, isoY);
}

sf::Vector2i Map::screenToTile(float x, float y) const {
    // Inverse isometric projection
    float tileX = (x / (tileWidth / 2.0f) + y / (tileHeight / 2.0f)) / 2.0f;
    float tileY = (y / (tileHeight / 2.0f) - x / (tileWidth / 2.0f)) / 2.0f;
    return sf::Vector2i(static_cast<int>(std::round(tileX)),
                        static_cast<int>(std::round(tileY)));
}