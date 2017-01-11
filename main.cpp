#include <iostream>
#include <deque>
#include <SDL.h>
#include "Apple.h"
#include "SnakeSegment.h"
#include "Snake.h"
#include "constants.h"

SDL_Window *displayWindow = nullptr;
SDL_Renderer *displayRenderer = nullptr;
SDL_Surface *displaySurface = nullptr;


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

    srand(SDL_GetTicks());

    Snake snake(headColor, segmentColor);
    Apple apple(appleColor);
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
                        if(snake.IsDead())
                            snake.Restart();
                    break;
            }
        }

        //Update
        snake.Update(&apple, &powerup);

        //Render
        SDL_SetRenderDrawColor(displayRenderer, 0, 0, 0, 255);
        SDL_RenderClear(displayRenderer);

        if(!snake.IsDead()){
            apple.Render(displayRenderer, displaySurface);
            powerup.Render(displayRenderer, displaySurface);
            snake.Render(displayRenderer, displaySurface);
        }

        SDL_RenderPresent(displayRenderer);
    }
    //}

    SDL_Quit();

    return 0;
}
