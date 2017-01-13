#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED

const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;
const unsigned int CELL_SIZE = 20;
const int CELL_WIDTH = SCREEN_WIDTH / CELL_SIZE;
const int CELL_HEIGHT = SCREEN_HEIGHT / CELL_SIZE;
const int FRAMES_PER_SECOND = 30;
const int SPEED_MULTIPLIER = 3;
const int SCORES_SHOWN = 10;

enum Directions{
    GO_NONE,
    GO_UP,
    GO_DOWN,
    GO_LEFT,
    GO_RIGHT
};

#endif // CONSTANTS_H_INCLUDED
