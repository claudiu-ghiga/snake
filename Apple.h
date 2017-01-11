#include <SDL.h>

#ifndef APPLE_H_INCLUDED
#define APPLE_H_INCLUDED

class Apple
{
    public:
    int x;
    int y;

    private:
    SDL_Rect renderRect;
    unsigned long appleColor;

    public:
    Apple(unsigned long appleColor);

    void Respawn();

    void Render(SDL_Renderer *renderer, SDL_Surface *surface);
};

#endif // APPLE_H_INCLUDED
