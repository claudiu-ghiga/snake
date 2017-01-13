#include <SDL.h>
#include "constants.h"

#ifndef APPLE_H_INCLUDED
#define APPLE_H_INCLUDED

class Snake;
class SnakeAI;

class Apple
{
    public:
    int x;
    int y;
    bool hasRespawned;

    private:
    SDL_Rect renderRect;
    unsigned long appleColor;

    public:
    Apple(unsigned long appleColor, bool gameMap[CELL_HEIGHT][CELL_WIDTH]);

    void Respawn(bool gameMap[CELL_HEIGHT][CELL_WIDTH]);

    void Render(SDL_Renderer *renderer, SDL_Surface *surface);
};

#endif // APPLE_H_INCLUDED
