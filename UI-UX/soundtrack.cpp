//
// Created by Nj on 4/11/2025.
//

#include "Soundtrack.h"
#include <iostream>
using namespace std;

Soundtrack::Soundtrack() {

}

bool Soundtrack::loadFromFile(const string &filename) {
    if (!music.openFromFile(filename)) {
        cerr << "Failed to load soundtrack" << endl;
        return false;
    }
    return true;
}

void Soundtrack::play() {
    music.setLoop(true);
    music.play();
}

void Soundtrack::stop() {
    music.stop();
}

void Soundtrack::setLoop(bool loop) {
    music.setLoop(loop);
}




