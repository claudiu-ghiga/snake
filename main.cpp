#include <iostream>
#include <deque>
#include <SDL.h>
#include "Apple.h"
#include "SnakeSegment.h"
#include "Snake.h"
#include "SnakeAI.h"
#include "constants.h"

SDL_Window *displayWindow = nullptr;
SDL_Renderer *displayRenderer = nullptr;
SDL_Surface *displaySurface = nullptr;

void UpdateMap(bool obstacles[CELL_HEIGHT][CELL_WIDTH], Snake *snake, SnakeAI *snakeAI);

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
    unsigned long AIHeadColor = SDL_MapRGB(displaySurface->format, 0, 240, 0);
    unsigned long AISegmentColor = SDL_MapRGB(displaySurface->format, 0, 160, 0);

    srand(SDL_GetTicks());

    bool gameMap[CELL_HEIGHT][CELL_WIDTH];
    Snake snake(headColor, segmentColor);
    SnakeAI computerSnake(AIHeadColor, AISegmentColor);
    UpdateMap(gameMap, &snake, &computerSnake);
    Apple apple(appleColor, gameMap);
    Powerup powerup;
    bool quit = false;
    SDL_Event sdlevent;

    //game loop //{
    while(!quit){

        //Input
        while(SDL_PollEvent(&sdlevent)){
            switch(sdlevent.type){
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    if(sdlevent.key.keysym.sym == SDLK_ESCAPE)
                        quit = true;
                    else if(sdlevent.key.keysym.sym == SDLK_RETURN)
                        if(snake.IsDead()){
                            snake.Restart();
                            computerSnake.Restart();
                            apple.Respawn(gameMap);
                        }
                    break;
            }
        }

        //Update
        UpdateMap(gameMap, &snake, &computerSnake);
        snake.Update(&apple, &powerup, gameMap);
        computerSnake.Update(&snake, &apple, gameMap);

        //Render
        SDL_SetRenderDrawColor(displayRenderer, 0, 0, 0, 255);
        SDL_RenderClear(displayRenderer);

        if(!snake.IsDead()){
            apple.Render(displayRenderer, displaySurface);
            powerup.Render(displayRenderer, displaySurface);
            computerSnake.Render(displayRenderer, displaySurface);
            snake.Render(displayRenderer, displaySurface);
        }

        SDL_RenderPresent(displayRenderer);
    }
    //}

    SDL_Quit();

    return 0;
}

void UpdateMap(bool obstacles[CELL_HEIGHT][CELL_WIDTH], Snake *snake, SnakeAI *computerSnake)
{
    for(int i = 0; i < CELL_HEIGHT; i++)
        for(int j = 0; j < CELL_WIDTH; j++){
            obstacles[i][j] = false;
            for(int k = 0; k < snake->segments.size(); k++)
                if(i == snake->segments[k].x && j == snake->segments[k].y)
                    obstacles[i][j] = true;
            for(int k = 0; k < computerSnake->segments.size(); k++)
                if(i == computerSnake->segments[k].x && j == computerSnake->segments[k].y)
                    obstacles[i][j] = true;
        }
}
