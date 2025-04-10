#include "../Map.h"
#include <sstream>

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

  return true;
}

bool Map::parseTilesets(XMLElement* mapElement) {
   for (XMLElement* tilesetElem = mapElement->FirstChildElement("tileset");
        tilesetElem != nullptr;
        tilesetElem = tilesetElem->NextSiblingElement("tileset")) {

       int firstGID = tilesetElem->IntAttribute("firstgid");

       const char* source = tilesetElem->Attribute("source");
       if (!source) {
           std::cerr << "No external tileset source found.\n";
           continue;
       }

       std::string tilesetPath = "maps/" + std::string(source); // adjust if needed


       // Load external .tsx file
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
           std::cerr << "❌ No image found in tileset: " << source << "\n";
           continue;
       }


       const char* imagePath = imageElem->Attribute("source");
       if (!imagePath) {
           std::cerr << "❌ Image tag found but no 'source' attribute.\n";
           continue;
       }
       std::string fullPath = "sprites/" + std::string(imagePath);  // path to spritesheet.png
       sf::Texture texture;
       if (!texture.loadFromFile(fullPath)) {
           std::cerr << "❌ Failed to load tilesheet: " << fullPath << "\n";
           continue;
       }
       tileTextures[firstGID] = texture;
       sf::Sprite sprite;
       sprite.setTexture(tileTextures[firstGID]);
       tileSprites[firstGID] = sprite;
       std::cout << "✅ Loaded tilesheet from: " << fullPath << "\n";
   }
   return true;
}










bool Map::parseLayers(XMLElement* mapElement, int width, int height) {
  for (XMLElement* layerElem = mapElement->FirstChildElement("layer");
       layerElem != nullptr;
       layerElem = layerElem->NextSiblingElement("layer")) {




      // Skip invisible layers (e.g. debugging, placeholders)
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
              } catch (const invalid_argument& e) {
                  cerr << "Invalid tile ID: \"" << token << "\" (not a number)\n";
              } catch (const out_of_range& e) {
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

    // Get the spritesheet texture (we assume the first tileset is used)
    auto it = tileTextures.begin();
    const sf::Texture& tilesheet = it->second;

    // Tileset configuration according to your .tsx file:
    //   tilewidth = 32, tileheight (actual art) = 64, margin = 2, spacing = 2, columns = 6
    const int margin  = 2;
    const int spacing = 2;
    const int columns = 6;

    // Logical tile dimensions for isometric math (from Tiled)
    // (Keep these as what Tiled sees in the map, e.g., 32x16)
    // Even though the actual art is 64 pixels tall.
    const int logicalTileWidth  = tileWidth;   // Should be 32.
    const int logicalTileHeight = tileHeight;  // Should be 16.

    // Actual art dimensions in the spritesheet
    const int actualTileWidth  = 32;
    const int actualTileHeight = 64;

    sf::Sprite sprite;
    sprite.setTexture(tilesheet);

    // Loop through all layers and draw each tile.
    for (const auto& layer : layers) {
        for (int y = 0; y < layer.height; ++y) {
            for (int x = 0; x < layer.width; ++x) {
                int index = x + y * layer.width;
                if (index >= static_cast<int>(layer.tileIDs.size()))
                    continue;

                unsigned tileID = layer.tileIDs[index];
                if (tileID == 0)
                    continue; // Skip empty tiles

                // Convert the 1-based GID to 0-based index.
                unsigned gid = tileID - 1;

                // Calculate the tile's position in the spritesheet (grid coordinates)
                int tx = gid % columns;
                int ty = gid / columns;

                // Calculate the texture coordinates, including margin and spacing.
                int textureX = margin + tx * (actualTileWidth + spacing);
                int textureY = margin + ty * (actualTileHeight + spacing);

                // Set the texture rectangle for the sprite (slicing out the entire 32×64 tile)
                sprite.setTextureRect(sf::IntRect(textureX, textureY, actualTileWidth, actualTileHeight));

                // ------------------------------------------------------------
                // Compute the isometric display position using logical tile sizes
                // ------------------------------------------------------------
                float isoX = static_cast<float>((x - y) * (logicalTileWidth / 2.0f));
                float isoY = static_cast<float>((x + y) * (logicalTileHeight / 2.0f));

                // Shift the sprite upward so that its "foot" aligns with the isometric grid.
                // (Actual tile height is 64 vs. logical height 16; offset = 64 - 16 = 48)
                isoY -= (actualTileHeight - logicalTileHeight);

                // Set position and draw the tile.
                sprite.setPosition(isoX, isoY);
                window.draw(sprite);
            }
        }
    }
}






int Map::getWidth() const {
  return layers.empty() ? 0 : layers[0].width;
}




int Map::getHeight() const {
  return layers.empty() ? 0 : layers[0].height;
}

