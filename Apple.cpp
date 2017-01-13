#include <cstdlib>
#include "Apple.h"

Apple::Apple(unsigned long appleColor, bool gameMap[CELL_HEIGHT][CELL_WIDTH])
{
    this->appleColor = appleColor;
    Apple::Respawn(gameMap);
    hasRespawned = true;
}

void Apple::Respawn(bool gameMap[CELL_HEIGHT][CELL_WIDTH])
{
    bool mustGenerate = true;
    while(mustGenerate){
        x = 1 + std::rand() % (CELL_WIDTH - 2);
        y = 1 + std::rand() % (CELL_HEIGHT - 2);
        mustGenerate = false;
        if(gameMap[x][y])
            mustGenerate = true;
    }
    hasRespawned = true;
}

void Apple::Render(SDL_Renderer *renderer, SDL_Surface *surface)
{
    renderRect.w = CELL_SIZE;
    renderRect.h = CELL_SIZE;
    renderRect.x = x * CELL_SIZE;
    renderRect.y = y * CELL_SIZE;

    Uint8 r = 0, g = 0, b = 0;

    SDL_GetRGB(appleColor, surface->format, &r, &g, &b);
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);

    SDL_RenderFillRect(renderer, &renderRect);
}
