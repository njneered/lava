#ifndef ASTAR_H
#define ASTAR_H

class AStar {
public:
    struct Node {
        int x;
        int y;
        float g;
        float h;
        float f;
        Node* parent;
    };
};



#endif //ASTAR_H
