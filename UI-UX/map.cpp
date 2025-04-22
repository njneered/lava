#include "Map.h"
#include <sstream>
#include <iostream>

using namespace tinyxml2;

using namespace std;

Map::Map() : tileWidth(32), tileHeight(16) {}

bool Map::loadFromFile(const std::string& tmxFilePath, int tw, int th) {
  tileWidth = tw;
  tileHeight = th;

  XMLDocument doc;
  if (doc.LoadFile(tmxFilePath.c_str()) != XML_SUCCESS) {
      cerr << "Failed to load TMX map file: " << tmxFilePath << "\n";
      return false;
  }

  XMLElement* mapElement = doc.FirstChildElement("map");
  if (mapElement == nullptr) {
      cerr << "No <map> element found.\n";
      return false;
  }

  int width = mapElement->IntAttribute("width");
  int height = mapElement->IntAttribute("height");

  if (!parseTilesets(mapElement)) return false;
  if (!parseLayers(mapElement, width, height)) return false;
    if (!lavaTexture.loadFromFile("sprites/lava-1.png")) {
        std::cerr << "Failed to load lava-1.png\n";
    } else {
        lavaSprite.setTexture(lavaTexture);
        generateSpreadingLava(100, 50); // number of lava tiles
    }

  return true;
}

bool Map::parseTilesets(XMLElement* mapElement) {
   for (XMLElement* tilesetElem = mapElement->FirstChildElement("tileset");
        tilesetElem != nullptr;
        tilesetElem = tilesetElem->NextSiblingElement("tileset")) {

       tilesetFirstGID = tilesetElem->IntAttribute("firstgid");

       const char* source = tilesetElem->Attribute("source");
       if (!source) {
           std::cerr << "No external tileset source found.\n";
           continue;
       }

       std::string tilesetPath = "maps/" + std::string(source);



       XMLDocument tsxDoc;
       if (tsxDoc.LoadFile(tilesetPath.c_str()) != XML_SUCCESS) {
           std::cerr << "Failed to load external tileset: " << tilesetPath << "\n";
           continue;
       }


       XMLElement* tsxRoot = tsxDoc.FirstChildElement("tileset");
       if (!tsxRoot) {
           std::cerr << "No <tileset> found in .tsx file.\n";
           continue;
       }


       XMLElement* imageElem = tsxRoot->FirstChildElement("image");
       if (!imageElem) {
           std::cerr << "No image found in tileset: " << source << "\n";
           continue;
       }


       const char* imagePath = imageElem->Attribute("source");
       if (!imagePath) {
           std::cerr << "Image tag found but no 'source' attribute.\n";
           continue;
       }

       std::string fullPath = "sprites/" + std::string(imagePath);  // path to spritesheet.png
       sf::Texture texture;
       if (!texture.loadFromFile(fullPath)) {
           std::cerr << "Failed to load tilesheet: " << fullPath << "\n";
           continue;
       }
       tileTextures[tilesetFirstGID] = texture;
       sf::Sprite sprite;
       sprite.setTexture(tileTextures[tilesetFirstGID]);
       tileSprites[tilesetFirstGID] = sprite;
       std::cout << "Loaded tilesheet from: " << fullPath << "\n";
   }
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

      std::vector<unsigned> tileIDs;
      std::stringstream ss(dataElem->GetText());
      std::string token;

      while (std::getline(ss, token, ',')) {
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
  }

  return true;
}

void Map::draw(sf::RenderWindow& window) const {
    if (tileTextures.empty()) return;

    auto it = tileTextures.begin();
    const sf::Texture& tilesheet = it->second;

    // tilewidth = 32, tileheight (actual art) = 64, margin = 2, spacing = 2, columns = 6
    const int margin  = 1;
    const int spacing = 2;
    const int columns = 6;
    const int actualTileWidth = 32;
    const int actualTileHeight = 64;

    const int logicalTileWidth  = tileWidth;
    const int logicalTileHeight = tileHeight;


    sf::Sprite sprite;
    sprite.setTexture(tilesheet);

    for (const auto& layer : layers) {
        for (int y = 0; y < layer.height; ++y) {
            for (int x = 0; x < layer.width; ++x) {
                int index = x + y * layer.width;
                if (index >= static_cast<int>(layer.tileIDs.size()))
                    continue;

                unsigned tileID = layer.tileIDs[index];
                if (tileID == 0)
                    continue;

                unsigned gid = tileID - tilesetFirstGID;

                int tx = gid % columns;
                int ty = gid / columns;

                int textureX = margin + tx * (actualTileWidth + spacing);
                int textureY = margin + ty * (actualTileHeight + spacing);

                sprite.setTextureRect(sf::IntRect(textureX, textureY, actualTileWidth, actualTileHeight));

                float isoX = static_cast<float>((x - y) * (logicalTileWidth / 2.0f));
                float isoY = static_cast<float>((x + y) * (logicalTileHeight / 2.0f));

                isoY -= (actualTileHeight - logicalTileHeight);

                sprite.setPosition(isoX, isoY);
                window.draw(sprite);
            }
        }
    }
    for (const auto& tile : lavaTiles) {
        float isoX = (tile.x - tile.y) * (tileWidth / 2.f);
        float isoY = (tile.x + tile.y) * (tileHeight / 2.f);
        sf::Sprite tempLavaSprite = lavaSprite;
        tempLavaSprite.setPosition(isoX, isoY);
        window.draw(tempLavaSprite);
    }
}

int Map::getWidth() const {
  return layers.empty() ? 0 : layers[0].width;
}

int Map::getHeight() const {
  return layers.empty() ? 0 : layers[0].height;
}

void Map::generateSpreadingLava(int seedCount, int initialLavaPerSeed) {
    lavaTiles.clear();
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    int w = getWidth();
    int h = getHeight();
    sf::Vector2i spawn(0, 0);

    std::vector<sf::Vector2i> seeds;

    // 1. Generate multiple lava seeds
    while (seeds.size() < static_cast<size_t>(seedCount)) {
        sf::Vector2i s(rand() % w, rand() % h);
        if (s != spawn && std::find(seeds.begin(), seeds.end(), s) == seeds.end()) {
            seeds.push_back(s);
            lavaTiles.push_back(s); // start from this tile
        }
    }

    // 2. For each seed, grow lava outward a bit
    std::vector<sf::Vector2i> directions = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1}
    };

    for (const auto& seed : seeds) {
        std::vector<sf::Vector2i> frontier = { seed };

        for (int i = 0; i < initialLavaPerSeed; ++i) {
            if (frontier.empty()) break;

            sf::Vector2i base = frontier[rand() % frontier.size()];
            sf::Vector2i dir = directions[rand() % directions.size()];
            sf::Vector2i next = base + dir;

            if (next.x < 0 || next.y < 0 || next.x >= w || next.y >= h)
                continue;

            if (next == spawn) continue;

            if (std::find(lavaTiles.begin(), lavaTiles.end(), next) == lavaTiles.end()) {
                lavaTiles.push_back(next);
                frontier.push_back(next);
            }
        }
    }
}


bool Map::isLava(int x, int y) const {
    return std::find(lavaTiles.begin(), lavaTiles.end(), sf::Vector2i(x, y)) != lavaTiles.end();
}

const std::vector<sf::Vector2i>& Map::getLavaTiles() const {
    return lavaTiles;
}

void Map::updateLavaSpread(float deltaTime) {
    lavaTimer += deltaTime;

    if (lavaTimer < 3.0f) return;
    lavaTimer = 0.0f;

    int w = getWidth();
    int h = getHeight();
    std::vector<sf::Vector2i> newLava;
    std::vector<sf::Vector2i> directions = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1}
    };

    int maxNewTiles = 10;
    int added = 0;

    for (const auto& tile : lavaTiles) {
        for (const auto& dir : directions) {
            sf::Vector2i next = tile + dir;

            if (next.x < 0 || next.y < 0 || next.x >= w || next.y >= h)
                continue;

            if (!isLava(next.x, next.y)) {
                newLava.push_back(next);
                added++;
                break; // spread 1 tile per existing lava tile
            }

            if (added >= maxNewTiles) break;
        }
        if (added >= maxNewTiles) break;
    }

    for (const auto& tile : newLava) {
        lavaTiles.push_back(tile);
    }
}
