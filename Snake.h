#include <deque>
#include <SDL.h>
#include "SnakeSegment.h"
#include "Apple.h"
#include "Powerup.h"
#include "constants.h"

#ifndef SNAKE_H_INCLUDED
#define SNAKE_H_INCLUDED

class Snake
{
    private:
    std::deque<SnakeSegment> segments;
    bool dead;
    unsigned int direction;
    unsigned int directionBeforeUpdate;
    unsigned int time, timeout;
    unsigned long headColor, segmentColor;
    SDL_Rect renderRect;
    unsigned int segmentsToAdd;
    unsigned long powerupTimeCounter;
    bool speedModified;
    bool sizeModified;
    unsigned long sizeRenderStartTime;
    bool scoreModified;
    int score;

    const int MOD_TIMEOUT = 100;
    const int NORMAL_TIMEOUT = 200;
    const int START_X = CELL_WIDTH / 2;
    const int START_Y = CELL_HEIGHT / 2;
    const unsigned int SPEED_POWERUP_TIME = 3000;
    const int SIZE_RENDER_TIME = 1000;

    enum PowerupType{
        SPEED,
        REDUCE_SIZE,
        BONUS
    };

    public:
    Snake(unsigned long headColor, unsigned long segmentColor);

    ~Snake();

    void Restart();

    void Update(Apple *apple, Powerup *powerup);

    bool IsDead();

    bool IsActive();

    void Render(SDL_Renderer *renderer, SDL_Surface *surface);

    private:
    void UpdateControls();

    void UpdateSnakeModifiers(Powerup *powerup);

    void AddSegment(unsigned int x, unsigned int y);

    void MoveSnake();

    bool CheckAppleCollision(Apple *apple);

    bool CheckPowerupCollision(Powerup *powerup);

    bool CheckSelfCollision();

    void UsePowerup(Powerup *powerup);
};

#endif // SNAKE_H_INCLUDED
