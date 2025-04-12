//
// Created by Nj on 4/11/2025.
//

#ifndef SOUNDTRACK_H
#define SOUNDTRACK_H

#include <SFML/Audio.hpp>
#include <string>
using namespace std;

class Soundtrack {
    sf::Music music;
public:
    Soundtrack();
    bool loadFromFile(const string& filename);
    void play();
    void stop();
    void setLoop(bool loop);
};

#endif //SOUNDTRACK_H
