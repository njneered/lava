//
// Created by Nj on 4/12/2025.
//

#ifndef UX_H
#define UX_H

#include <SFML/Graphics.hpp>
#include <string>
using namespace std;

class UX {
    sf::Font font;
    sf::Text text;
    sf::Text secondaryText;
    sf::Text instructionText;
    bool typingActive;
    sf::RectangleShape inputBox;
    sf::Text inputText;
    std::string typedCoords;
    sf::RectangleShape cursor;
    sf::Clock cursorTimer;
    bool cursorVisible;

public:
    UX(const string& message);
    void setMessage(const string& message);
    void setSecondaryMessage(const string& message);
    void setInstructionMessage(const string& message);
    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    bool hasValidInput() const;
    sf::Vector2i getTypedCoordinates() const;
    void resetTypedCoordinates();
    void draw(sf::RenderWindow& window);
    void updateCursor();
};

#endif //UX_H
