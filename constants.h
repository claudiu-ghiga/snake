#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED

const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;
const unsigned int CELL_SIZE = 20;
const int CELL_WIDTH = SCREEN_WIDTH / CELL_SIZE;
const int CELL_HEIGHT = SCREEN_HEIGHT / CELL_SIZE;
const unsigned int START_LENGTH = 5;   //minimum = 1

enum Directions{
    GO_NONE,
    GO_UP,
    GO_DOWN,
    GO_LEFT,
    GO_RIGHT
};

#endif // CONSTANTS_H_INCLUDED
