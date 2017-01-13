#include <SDL.h>

#ifndef POWERUP_H_INCLUDED
#define POWERUP_H_INCLUDED

class Powerup
{
    public:
    int x;
    int y;
    unsigned long timeout;
    unsigned long time;
    bool powerupDeactivated;

    private:
    SDL_Rect renderRect;
    bool mustRender;
    unsigned char type;

    public:
    Powerup();

    void Respawn();

    void Despawn();

    unsigned int getType();

    void Render(SDL_Renderer *renderer, SDL_Surface *surface);
};

#endif // POWERUP_H_INCLUDED

