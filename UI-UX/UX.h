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
    sf::Text inputText;
    sf::Text coordConfirmText;

    sf::RectangleShape inputBox;
    sf::RectangleShape confirmBackground;
    sf::RectangleShape cursor;

    std::string typedCoords;
    std::string coordMessage;

    sf::Clock cursorTimer;

    bool typingActive;
    bool cursorVisible;

public:
    UX(const string& message);

    void setMessage(const string& message);
    void setSecondaryMessage(const string& message);
    void setInstructionMessage(const string& message);

    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    bool hasValidInput() const;
    void resetTypedCoordinates();
    void draw(sf::RenderWindow& window);

    void updateCursor();
    void setConfirmationMessage(const std::string& message);
    void clearConfirmationMessage();
    bool isTypingActive() const { return typingActive; }

    sf::Vector2i getTypedCoordinates() const;
};

#endif
