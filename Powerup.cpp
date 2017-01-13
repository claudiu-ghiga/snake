#include "Powerup.h"
#include "constants.h"
#include <cstdlib>

Powerup::Powerup()
{
    type = std::rand() % 3;
    timeout = (10 + rand() % 6) * 500;
    Powerup::Despawn();
}

void Powerup::Respawn()
{
    type = std::rand() % 3;
    x = 1 + std::rand() % (CELL_WIDTH - 2);
    y = 1 + std::rand() % (CELL_HEIGHT - 2);
    mustRender = true;
    powerupDeactivated = false;
}

void Powerup::Despawn()
{
    x = -1;
    y = -1;
    mustRender = false;
    time = SDL_GetTicks();
    powerupDeactivated = true;
}

unsigned int Powerup::getType()
{
    return type;
}

void Powerup::Render(SDL_Renderer *renderer, SDL_Surface *surface)
{
    if(mustRender){
        renderRect.h = CELL_SIZE;
        renderRect.w = CELL_SIZE;
        renderRect.x = x * CELL_SIZE;
        renderRect.y = y * CELL_SIZE;

        unsigned char r = rand() % 256;
        unsigned char g = rand() % 256;
        unsigned char b = rand() % 256;
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);

        SDL_RenderFillRect(renderer, &renderRect);
    }
}

