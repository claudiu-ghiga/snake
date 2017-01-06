#include <iostream>
#include <deque>
#include "SDL/SDL.h"

SDL_Window *displayWindow = nullptr;
SDL_Renderer *displayRenderer = nullptr;
SDL_Surface *displaySurface = nullptr;

const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;
const unsigned int CELL_SIZE = 20;
const int CELL_WIDTH = SCREEN_WIDTH / CELL_SIZE;
const int CELL_HEIGHT = SCREEN_HEIGHT / CELL_SIZE;
const unsigned int START_X = CELL_WIDTH / 2;
const unsigned int START_Y = CELL_HEIGHT / 2;
const unsigned int START_LENGTH = 3;   //minimum = 1

enum Directions{
    GO_NONE,
    GO_UP,
    GO_DOWN,
    GO_LEFT,
    GO_RIGHT
};

class SnakeSegment
{
    public:
    int x;
    int y;

    SnakeSegment(int x, int y)
    {
        this->x = x;
        this->y = y;
    }
};

class Apple
{
    public:
    unsigned int x;
    unsigned int y;

    Apple()
    {
        Respawn();
    }

    void Respawn()
    {
        x = 1 + rand() % (CELL_WIDTH - 2);
        y = 1 + rand() % (CELL_HEIGHT - 2);
    }
};

class Snake
{
    private:
    std::deque<SnakeSegment> segments;
    Apple apple;
    bool dead;
    unsigned int direction;
    unsigned int directionBeforeUpdate;
    unsigned time, timeout;
    unsigned long headColor, segmentColor, appleColor;
    SDL_Rect renderRect;
    unsigned int segmentsToAdd;

    public:
    Snake(unsigned long headColor, unsigned long segmentColor, unsigned long appleColor)
    {
        this->headColor = headColor;
        this->segmentColor = segmentColor;
        this->appleColor = appleColor;
        Restart();
    }

    ~Snake()
    {
        segments.clear();
    }

    void Restart()
    {
        segments.clear();
        //for(unsigned int i = 0; i < START_LENGTH; i++)
            AddSegment(START_X /*- i*/, START_Y);
        direction = GO_NONE;
        directionBeforeUpdate = direction;
        segmentsToAdd = START_LENGTH;
        time = 0;
        timeout = 150;
        dead = false;
    }

    void Update()
    {
        if(dead)
            return;

        UpdateControls();

        time++;
        if(time < timeout)
            return;

        directionBeforeUpdate = direction;

        time = 0;

        if(CheckSelfCollision()){
            dead = true;
            return;
        }

        if(CheckAppleCollision())
            apple.Respawn();
        else{
            if(direction != GO_NONE && (segments.size() > 1))
                segments.pop_back();
        }

        int oldX = segments.back().x;
        int oldY = segments.back().y;

        MoveSnake();

        if(direction != GO_NONE && segmentsToAdd > 0){
            segmentsToAdd--;
            if(segments.size() <= START_LENGTH)
                AddSegment(oldX, oldY);
        }
    }

    bool IsDead()
    {
        return dead;
    }

    void Render(SDL_Renderer *renderer)
    {
        if(dead)
            return ;
        renderRect.w = CELL_SIZE;
        renderRect.h = CELL_SIZE;

        RenderApple(renderer);
        RenderSnake(renderer);
    }

    private:
    void UpdateControls()
    {
        const unsigned char *keys = SDL_GetKeyboardState(nullptr);

        if(keys[SDL_SCANCODE_UP] && direction != GO_DOWN &&
            directionBeforeUpdate != GO_DOWN){
            direction = GO_UP;
        }
        else if(keys[SDL_SCANCODE_DOWN] && direction != GO_UP &&
                directionBeforeUpdate != GO_UP){
            direction = GO_DOWN;
        }
        else if(keys[SDL_SCANCODE_LEFT] && direction != GO_RIGHT &&
                directionBeforeUpdate != GO_RIGHT){
            direction = GO_LEFT;
        }
        else if(keys[SDL_SCANCODE_RIGHT] && direction != GO_LEFT &&
                directionBeforeUpdate != GO_LEFT){
            direction = GO_RIGHT;
        }
    }

    void RenderSnake(SDL_Renderer *renderer)
    {
        renderRect.x = segments[0].x * CELL_SIZE;
        renderRect.y = segments[0].y * CELL_SIZE;

        Uint8 r = 0, g = 0, b = 0;

        SDL_GetRGB(headColor, displaySurface->format, &r, &g, &b);
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);

        SDL_RenderFillRect(renderer, &renderRect);

        SDL_GetRGB(segmentColor, displaySurface->format, &r, &g, &b);
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);

        for(unsigned int i = 1; i < segments.size(); i++){
            renderRect.x = segments[i].x * CELL_SIZE;
            renderRect.y = segments[i].y * CELL_SIZE;
            SDL_RenderFillRect(renderer, &renderRect);
        }
    }

    void RenderApple(SDL_Renderer *renderer)
    {
        renderRect.x = apple.x * CELL_SIZE;
        renderRect.y = apple.y * CELL_SIZE;

        Uint8 r = 0, g = 0, b = 0;

        SDL_GetRGB(appleColor, displaySurface->format, &r, &g, &b);
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);

        SDL_RenderFillRect(renderer, &renderRect);
    }

    void AddSegment(unsigned int x, unsigned int y)
    {
        SnakeSegment segment(x, y);
        segments.push_back(segment);
    }
    void MoveSnake()
    {
        if(direction != GO_NONE){
            static const int moveX[] = {0, 0, 0, -1, 1};
            static const int moveY[] = {0, -1, 1, 0, 0};

            int x = (segments[0].x + moveX[direction])%CELL_WIDTH;
            if(x < 0)
                x = CELL_WIDTH - 1;
            int y = (segments[0].y + moveY[direction])%CELL_HEIGHT;
            if(y < 0)
                y = CELL_HEIGHT - 1;

            SnakeSegment nextSegment(x, y);
            segments.push_front(nextSegment);
        }
    }

    bool CheckAppleCollision()
    {
        return (segments[0].x == apple.x && segments[0].y == apple.y);
    }

    bool CheckSelfCollision()
    {
        unsigned int headX = segments[0].x;
        unsigned int headY = segments[0].y;

        if(segments.size() > 1)
            for(unsigned int i = 1; i < segments.size(); i++)
                if(segments[i].x == headX && segments[i].y == headY)
                    return true;
        return false;
    }

};

int main(int argc, char *argv[])
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cout<<"SDL_Init failed: "<<SDL_GetError()<<std::endl;
        return -1;
    }

    displayWindow = SDL_CreateWindow("Snake Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                      SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if(displayWindow == nullptr){
        std::cout<<"SDL could not create window: "<<SDL_GetError()<<std::endl;
        return -1;
    }

    displayRenderer = SDL_CreateRenderer(displayWindow, 0, SDL_RENDERER_ACCELERATED);
    if(displayRenderer == nullptr){
        std::cout<<"SDL could not create renderer: "<<SDL_GetError()<<std::endl;
        return -1;
    }

    displaySurface = SDL_GetWindowSurface(displayWindow);

    //background color = 0, 0, 0
    //snake head color = 180, 180, 180
    //snake body color = 120, 120, 120
    //apple color = 255, 0, 0

    unsigned long headColor = SDL_MapRGB(displaySurface->format, 180, 180, 180);
    unsigned long segmentColor = SDL_MapRGB(displaySurface->format, 120, 120, 120);
    unsigned long appleColor = SDL_MapRGB(displaySurface->format, 255, 0, 0);

    Snake snake(headColor, segmentColor, appleColor);
    bool quit = false;
    SDL_Event sdlevent;

    //game loop //{
    while(!quit){

        while(SDL_PollEvent(&sdlevent)){
            switch(sdlevent.type){
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    if(sdlevent.key.keysym.sym == SDLK_ESCAPE)
                        quit = true;
                    else if(sdlevent.key.keysym.sym == SDLK_RETURN)
                        if(snake.IsDead())
                            snake.Restart();
                    break;
            }
        }

        snake.Update();

        SDL_SetRenderDrawColor(displayRenderer, 0, 0, 0, 255);
        SDL_RenderClear(displayRenderer);

        snake.Render(displayRenderer);

        SDL_RenderPresent(displayRenderer);
    }
    //}

    SDL_Quit();

    return 0;
}
