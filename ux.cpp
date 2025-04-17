//
// Created by Nj on 4/12/2025.
//

#include "UX.h"
#include <iostream>
using namespace std;

UX::UX(const string& message) {
    if (!font.loadFromFile("font/aesymatt.ttf")) {
        cerr << "Failed to load font" << endl;
    }
    text.setFont(font);
    text.setString(message);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color(219, 18, 118));
    text.setPosition(10.f, 10.f);

    initializeButtons();
}

void UX::setMessage(const string &message) {
    text.setString(message);
}

void UX::initializeButtons() {
    if (!buttonsTexture.loadFromFile("sprites/buttons.png")) {
        cerr << "Failed to load buttons" << endl;
    }

    redButton.setTexture(buttonsTexture);
    redButton.setTextureRect(sf::IntRect(0, 0, 16, 16));
    redButton.setScale(2.f, 2.f);

    greenButton.setTexture(buttonsTexture);
    greenButton.setTextureRect(sf::IntRect(0, 16, 16, 16));
    greenButton.setScale(2.f, 2.f);
}


void UX::draw(sf::RenderWindow &window) {
    // Get the window width (assumed fixed 1280, but recalculated for safety).
    sf::Vector2u winSize = window.getSize();
    float winWidth = static_cast<float>(winSize.x);

    // --- Draw the Text ---
    // First, compute local bounds of the text.
    sf::FloatRect textLocal = text.getLocalBounds();
    // Set the origin so the text is centered horizontally (and top stays at y=0).
    text.setOrigin(textLocal.left + textLocal.width / 2.f, 0.f);
    // Position the text at the top-center of the window (20 pixels from the top).
    text.setPosition(winWidth / 2.f, 20.f);

    // Draw the text.
    window.draw(text);

    // --- Draw the Buttons Underneath the Text ---
    // Use the global bounds of the text (after setting position) to get its final location.
    sf::FloatRect textGlobal = text.getGlobalBounds();
    // Decide on a gap below the text—for example, 20 pixels below the bottom of the text.
    float gap = 20.f;
    float buttonsY = textGlobal.top + textGlobal.height + gap;

    // Our buttons are 16x16 scaled by 2 (so 32x32). We want them side-by-side, centered.
    const float buttonWidth = 32.f; // 16 scaled by 2
    const float spacing = 20.f;     // Spacing between the two buttons
    float totalButtonsWidth = 2 * buttonWidth + spacing;

    // Compute the starting X coordinate so the group is centered.
    float startX = (winWidth - totalButtonsWidth) / 2.f;

    // Set positions for the two buttons.
    redButton.setPosition(startX, buttonsY);
    greenButton.setPosition(startX + buttonWidth + spacing, buttonsY);

    sf::FloatRect greenBounds = greenButton.getGlobalBounds();
    // Create a debug outline that matches the button's size.
    sf::RectangleShape debugOutline(sf::Vector2f(greenBounds.width, greenBounds.height));
    debugOutline.setPosition(greenBounds.left, greenBounds.top);
    debugOutline.setFillColor(sf::Color::Transparent);     // No fill
    debugOutline.setOutlineColor(sf::Color::Red);            // Red outline
    debugOutline.setOutlineThickness(1.f);

    // Draw the debug outline so you can see where the button is supposed to be.
    window.draw(debugOutline);

    // Draw both buttons.
    window.draw(redButton);
    window.draw(greenButton);
}


