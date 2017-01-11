#include <cstdlib>
#include "constants.h"
#include "Apple.h"

Apple::Apple(unsigned long appleColor)
{
    this->appleColor = appleColor;
    Apple::Respawn();
}

void Apple::Respawn()
{
    x = 1 + std::rand() % (CELL_WIDTH - 2);
    y = 1 + std::rand() % (CELL_HEIGHT - 2);
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
