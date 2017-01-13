#include <deque>
#include <SDL.h>
#include "SnakeSegment.h"
#include "Apple.h"
#include "Snake.h"

#ifndef SNAKEAI_H_INCLUDED
#define SNAKEAI_H_INCLUDED

class SnakeAI
{
    public:
    std::deque<SnakeSegment> segments;

    private:
    struct Cell{
        int x;
        int y;
        Cell *parent;
    };

    std::deque<int> dirs;
    std::deque<int> dirsCopy;
    bool dead;
    int direction;
    unsigned time, timeout;
    unsigned long headColor, segmentColor;
    SDL_Rect renderRect;
    unsigned int segmentsToAdd;
    int score;

    const int AI_START_X = CELL_WIDTH / 4;
    const int AI_START_Y = CELL_HEIGHT / 2;
    const unsigned int START_LENGTH = 3;

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
    SnakeAI(unsigned long headColor, unsigned long segmentColor);

    ~SnakeAI();

    void Restart();

    void Update(Snake *playerSnake, Apple *apple, bool gameMap[CELL_HEIGHT][CELL_WIDTH]);

    bool IsDead();

    void Render(SDL_Renderer *renderer, SDL_Surface *surface);

    private:
    void AddSegment(unsigned int x, unsigned int y);

    int ComputeDirection(std::deque<int> &directionsQueue);

    std::deque<int> ComputePath(Apple *apple);

    bool IsValidDir(Cell c);

    void MoveSnake();

    bool CheckAppleCollision(Apple *apple);
};

#endif // SNAKEAI_H_INCLUDED
