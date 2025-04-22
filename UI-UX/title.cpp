#include "Title.h"
#include <sstream>
#include <iostream>
using namespace std;

Title::Title() : finished(false) {
    if (!font.loadFromFile("font/aesymatt.ttf")) {
        std::cerr << "Failed to load font" << std::endl;
    }

    titleText.setFont(font);
    titleText.setString("Welcome to the Floor is Lava!");
    titleText.setCharacterSize(72);
    titleText.setFillColor(sf::Color(219, 18, 118));
    titleText.setStyle(sf::Text::Bold);

    subtitleText.setFont(font);
    subtitleText.setString("A Game by Reptile Dysfunction (Group 77)");
    subtitleText.setCharacterSize(24);
    subtitleText.setFillColor(sf::Color(219, 18, 118));
    subtitleText.setStyle(sf::Text::Bold);

    instructionText.setFont(font);
    instructionText.setString("Click anywhere to continue.");
    instructionText.setCharacterSize(48);
    instructionText.setFillColor(sf::Color(219, 18, 118));
    instructionText.setStyle(sf::Text::Regular);


    float windowWidth = 1280.f;
    float yTitle = 100.f;
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    float ySubtitle = yTitle + titleBounds.height + 50.f;
    sf::FloatRect subBounds = subtitleText.getLocalBounds();
    float yInstruction = ySubtitle + subBounds.height + 200.f;

    // Set the origins so that texts are centered horizontally.
    sf::FloatRect tb = titleText.getLocalBounds();
    titleText.setOrigin(tb.left + tb.width / 2.0f, 0.f);
    titleText.setPosition(windowWidth / 2.0f, yTitle);

    sf::FloatRect sb = subtitleText.getLocalBounds();
    subtitleText.setOrigin(sb.left + sb.width / 2.0f, 0.f);
    subtitleText.setPosition(windowWidth / 2.0f, ySubtitle);

    sf::FloatRect ib = instructionText.getLocalBounds();
    instructionText.setOrigin(ib.left + ib.width / 2.0f, 0.f);
    instructionText.setPosition(windowWidth / 2.0f, yInstruction);
}
void Title::handleEvent(const sf::Event &event) {
    if (event.type == sf::Event::MouseButtonPressed)
        finished = true;
}

void Title::draw(sf::RenderWindow& window) {
    window.draw(titleText);
    window.draw(subtitleText);
    window.draw(instructionText);
}

bool Title::isFinished() const {
    return finished;
}
