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
    public:
    std::deque<SnakeSegment> segments;

    private:
    bool dead;
    int speed;
    unsigned int direction;
    unsigned int directionBeforeUpdate;
    unsigned long headColor, segmentColor;
    SDL_Rect renderRect;
    unsigned int segmentsToAdd;
    unsigned long powerupTimeCounter;
    bool speedModified;
    bool sizeModified;
    unsigned long sizeRenderStartTime;
    bool scoreModified;
    int score;

    const int MOD_SPEED = 2;
    const int NORMAL_SPEED = 1;
    const int START_X = (3 * CELL_WIDTH) / 4;
    const int START_Y = CELL_HEIGHT / 2;
    const unsigned int SPEED_POWERUP_TIME = 3000;
    const int SIZE_RENDER_TIME = 1000;
    const unsigned int START_LENGTH = 3;
    const int SEGMENTS_TO_REMOVE = 5;

    enum PowerupType{
        SPEED,
        REDUCE_SIZE,
        BONUS
    };

    struct Timer{

        unsigned long startTicks;

        Timer()
        {
            startTicks = 0;
        }

        void Start()
        {
            startTicks = SDL_GetTicks();
        }

        unsigned long GetTicks()
        {
            return SDL_GetTicks() - startTicks;
        }
    } timer;

    public:
    Snake(unsigned long headColor, unsigned long segmentColor);

    ~Snake();

    void Restart();

    void Update(Apple *apple, Powerup *powerup, bool gameMap[CELL_HEIGHT][CELL_WIDTH]);

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
