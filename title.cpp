//
// Created by Nj on 4/11/2025.
//

#include "Title.h"
#include <sstream>
#include <iostream>
using namespace std;

Title::Title(const string& message) : finished(false) {
    if (!font.loadFromFile("font/aesymatt.ttf")) {
        std::cerr << "Failed to load font" << std::endl;
    }

    istringstream iss(message);
    string lineStr;
    while(getline(iss, lineStr)) {
        if (!lineStr.empty() && lineStr.back() == '\r')
            lineStr.pop_back();
        sf::Text t;
        t.setFont(font);
        t.setString(lineStr);
        t.setCharacterSize(48);
        t.setFillColor(sf::Color(227, 125, 77)); // Use a custom color if desired.
        t.setStyle(sf::Text::Bold);
        lines.push_back(t);
    }

    const float spacing = 10.f;
    float totalHeight = 0.f;
    for (auto& t : lines) {
        sf::FloatRect bounds = t.getLocalBounds();
        totalHeight += bounds.height;
    }
    totalHeight += spacing * (lines.size() - 1);

    float windowWidth = 1280.f;
    float windowHeight = 720.f;
    float startY = (windowHeight - totalHeight) / 2.f;

    for (auto& t : lines) {
        sf::FloatRect bounds = t.getLocalBounds();
        t.setOrigin(bounds.left + bounds.width / 2.0f,
                    bounds.top + bounds.height / 2.0f);
        t.setPosition(windowWidth / 2.0f, startY + bounds.height / 2.0f);
        startY += bounds.height + spacing;
    }
}

void Title::handleEvent(const sf::Event &event) {
    if (event.type == sf::Event::MouseButtonPressed)
        finished = true;
}

void Title::draw(sf::RenderWindow& window) {
    const float padding = 10.f;
    for (auto& t : lines) {
        sf::FloatRect bounds = t.getGlobalBounds();
        sf::RectangleShape rect(sf::Vector2f(bounds.width + padding * 2, bounds.height + padding * 2));
        rect.setFillColor(sf::Color(255, 255, 255, 200)); // translucent white
        float rectX = t.getPosition().x - bounds.width / 2.f - padding;
        float rectY = t.getPosition().y - padding;
        rect.setPosition(rectX, rectY);

        window.draw(rect);
        window.draw(t);
    }
}

bool Title::isFinished() const {
    return finished;
}
