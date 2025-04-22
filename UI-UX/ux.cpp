
#include "UX.h"
#include <iostream>
#include <sstream>
using namespace std;

UX::UX(const string& message) {
    if (!font.loadFromFile("font/aesymatt.ttf")) {
        cerr << "Failed to load font" << endl;
    }
    text.setFont(font);
    text.setString(message);
    text.setCharacterSize(30);
    text.setFillColor(sf::Color(219, 18, 118));
    text.setPosition(10.f, 10.f);

    secondaryText.setFont(font);
    secondaryText.setString("Press 1 for Dijkstra's, and 2 for A*");
    secondaryText.setCharacterSize(24);
    secondaryText.setFillColor(sf::Color(219, 18, 118));

    sf::FloatRect mainBounds = text.getGlobalBounds();
    secondaryText.setPosition(text.getPosition().x, text.getPosition().y + mainBounds.height + 8.f);

    instructionText.setFont(font);
    instructionText.setCharacterSize(24);
    instructionText.setFillColor(sf::Color(180, 180, 180));
    instructionText.setStyle(sf::Text::Italic);
    instructionText.setString("Zoom out and click on the map where you want to go.");

    // Position instruction text below secondary text
    sf::FloatRect secondaryBounds = secondaryText.getGlobalBounds();
    instructionText.setPosition(secondaryText.getPosition().x, secondaryText.getPosition().y + secondaryBounds.height + 8.f);

    typingActive = false;
    typedCoords = "";

    inputBox.setSize(sf::Vector2f(200.f, 30.f));
    inputBox.setFillColor(sf::Color(0, 0, 0, 180));
    inputBox.setPosition(10.f, 140.f);
    inputBox.setOutlineThickness(2.f);
    inputBox.setOutlineColor(sf::Color::White);

    inputText.setFont(font);
    inputText.setCharacterSize(16);
    inputText.setFillColor(sf::Color::White);
    inputText.setPosition(inputBox.getPosition().x + 8.f, inputBox.getPosition().y + 5.f);
    inputText.setString("");

    cursor.setSize(sf::Vector2f(2.f, 20.f));
    cursor.setFillColor(sf::Color::White);
    cursorVisible = true;
    cursorTimer.restart();

    coordConfirmText.setFont(font);
    coordConfirmText.setCharacterSize(18);
    coordConfirmText.setFillColor(sf::Color(30, 30, 30));
    coordConfirmText.setPosition(10.f, 180.f);
}

void UX::setMessage(const string &message) {
    text.setString(message);

    sf::FloatRect mainBounds = text.getGlobalBounds();
    secondaryText.setPosition(text.getPosition().x,
                              text.getPosition().y + mainBounds.height + 8.f);

    sf::FloatRect secondaryBounds = secondaryText.getGlobalBounds();
    instructionText.setPosition(secondaryText.getPosition().x,
                                secondaryText.getPosition().y + secondaryBounds.height + 8.f);

}

void UX::setSecondaryMessage(const string &message) {
    secondaryText.setString(message);

    sf::FloatRect secondaryBounds = secondaryText.getGlobalBounds();
    instructionText.setPosition(secondaryText.getPosition().x,
                                secondaryText.getPosition().y + secondaryBounds.height + 8.f);
}

void UX::setInstructionMessage(const std::string& message) {
    instructionText.setString(message);
}


void UX::draw(sf::RenderWindow &window) {
    window.draw(text);
    sf::FloatRect mainBounds = text.getGlobalBounds();
    sf::RectangleShape mainBackground(sf::Vector2f(mainBounds.width + 10.f, mainBounds.height + 6.f));
    mainBackground.setFillColor(sf::Color(255, 255, 255, 200));
    mainBackground.setPosition(mainBounds.left - 5.f, mainBounds.top - 3.f);
    window.draw(mainBackground);
    window.draw(text);

    sf::FloatRect secondaryBounds = secondaryText.getGlobalBounds();
    sf::RectangleShape secondaryBackground(sf::Vector2f(secondaryBounds.width + 10.f, secondaryBounds.height + 6.f));
    secondaryBackground.setFillColor(sf::Color(255, 255, 255, 200));
    secondaryBackground.setPosition(secondaryBounds.left - 5.f, secondaryBounds.top - 3.f);
    window.draw(secondaryBackground);
    window.draw(secondaryText);

    sf::FloatRect instructBounds = instructionText.getGlobalBounds();
    sf::RectangleShape instructBG(sf::Vector2f(instructBounds.width + 10.f, instructBounds.height + 6.f));
    instructBG.setFillColor(sf::Color(255, 255, 255, 200));
    instructBG.setPosition(instructBounds.left - 5.f, instructBounds.top - 3.f);
    window.draw(instructBG);
    window.draw(instructionText);
    window.draw(inputBox);
    window.draw(inputText);

    if (typingActive && cursorVisible) {
        window.draw(cursor);
    }
    if (!coordMessage.empty()) {
        window.draw(confirmBackground);
        window.draw(coordConfirmText);
    }

}

void UX::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
        sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);

        typingActive = inputBox.getGlobalBounds().contains(worldPos);
    }

    if (typingActive && event.type == sf::Event::TextEntered) {
        char c = static_cast<char>(event.text.unicode);
        if (isdigit(c) || c == ' ') {
            typedCoords += c;
        } else if (event.text.unicode == 8 && !typedCoords.empty()) {
            typedCoords.pop_back(); // Backspace
        }

        inputText.setString(typedCoords);
    }
}

bool UX::hasValidInput() const {
    int x, y;
    std::istringstream ss(typedCoords);
    return static_cast<bool>(ss >> x >> y);
}

sf::Vector2i UX::getTypedCoordinates() const {
    int x = 0, y = 0;
    std::istringstream ss(typedCoords);
    ss >> x >> y;
    return {x, y};
}

void UX::resetTypedCoordinates() {
    typedCoords.clear();
    inputText.setString("");
    typingActive = false;
}

void UX::updateCursor() {
    if (cursorTimer.getElapsedTime().asSeconds() >= 0.5f) {
        cursorVisible = !cursorVisible;
        cursorTimer.restart();
    }

    sf::FloatRect textBounds = inputText.getGlobalBounds();
    cursor.setPosition(inputText.getPosition().x + textBounds.width + 2.f, inputText.getPosition().y);

}

void UX::setConfirmationMessage(const std::string& message) {
    coordMessage = message;
    coordConfirmText.setString(message);

    float padding = 10.f;
    sf::FloatRect bounds = coordConfirmText.getLocalBounds();
    coordConfirmText.setOrigin(bounds.left, bounds.top);
    coordConfirmText.setPosition(10.f + padding, 200.f); // Adjust Y if needed

    confirmBackground.setSize(sf::Vector2f(bounds.width + 2 * padding, bounds.height + 2 * padding));
    confirmBackground.setFillColor(sf::Color(255, 255, 255, 180)); // White translucent
    confirmBackground.setPosition(coordConfirmText.getPosition().x - padding, coordConfirmText.getPosition().y - padding);
}

void UX::clearConfirmationMessage() {
    coordMessage.clear();
    coordConfirmText.setString("");
    confirmBackground.setSize(sf::Vector2f(0, 0));
}
