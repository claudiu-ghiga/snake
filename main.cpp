#include <iostream>
#include <deque>
#include <cstdio>
#include <fstream>
#include <string>
#include <iomanip>
#include <conio.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include "Apple.h"
#include "SnakeSegment.h"
#include "Snake.h"
#include "SnakeAI.h"
#include "constants.h"

std::fstream scoreFile;

struct Score{
    std::string name;
    int score;
};

SDL_Window *displayWindow = nullptr;
SDL_Renderer *displayRenderer = nullptr;
SDL_Surface *displaySurface = nullptr;
SDL_Surface *endScreen = nullptr;

void UpdateMap(bool obstacles[CELL_HEIGHT][CELL_WIDTH], Snake *snake, SnakeAI *snakeAI);
void ReadScores(Score scores[SCORES_SHOWN]);
void WriteScores(Score scores[SCORES_SHOWN], std::string nameToAdd, int scoreToAdd);

int main(int argc, char *argv[])
{
    bool quitMenu = false;
    int userInput;
    Score scores[SCORES_SHOWN];
    ReadScores(scores);

    while(!quitMenu){

        std::cout<<"------------------SNAKE------------------\n";
        std::cout<<'\n';

        std::cout<<"Introduceti numarul:\n1 pentru single player\n2 pentru un joc impotriva calculatorului\n3 pentru a vizualiza scorurile\n-1 pentru a iesi din joc\n";

        std::cin>>userInput;

        if(userInput == 1 || userInput == 2){

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
            SnakeAI computerSnake(AIHeadColor, AISegmentColor, userInput);
            UpdateMap(gameMap, &snake, &computerSnake);
            Apple apple(appleColor, gameMap);
            Powerup powerup;
            bool quitGame = false;
            SDL_Event sdlevent;

            while(!quitGame){

                //Input
                while(SDL_PollEvent(&sdlevent)){
                    switch(sdlevent.type){
                        case SDL_QUIT:
                            quitGame = true;
                            break;
                        case SDL_KEYDOWN:
                            if(sdlevent.key.keysym.sym == SDLK_ESCAPE)
                                quitGame = true;
                            else if(sdlevent.key.keysym.sym == SDLK_RETURN)
                                if(snake.IsDead()){
                                    snake.Restart();
                                    if(userInput == 2)
                                        computerSnake.Restart();
                                    apple.Respawn(gameMap);
                                }
                            break;
                    }
                }

                if(snake.IsDead() || computerSnake.IsDead())
                    quitGame = true;
                //Update
                if(!snake.IsDead() && !computerSnake.IsDead()){
                    UpdateMap(gameMap, &snake, &computerSnake);
                    snake.Update(&apple, &powerup, gameMap);
                    if(userInput == 2)
                        computerSnake.Update(&snake, &apple, gameMap);
                }

                //Render
                SDL_SetRenderDrawColor(displayRenderer, 0, 0, 0, 255);
                SDL_RenderClear(displayRenderer);

                if(!snake.IsDead() && !computerSnake.IsDead()){
                    apple.Render(displayRenderer, displaySurface);
                    if(userInput == 2)
                        computerSnake.Render(displayRenderer, displaySurface);
                    powerup.Render(displayRenderer, displaySurface);
                    snake.Render(displayRenderer, displaySurface);
                }

                SDL_RenderPresent(displayRenderer);
            }

            if(userInput == 2){
                system("cls");
                if((snake.score > computerSnake.score) || computerSnake.IsDead())
                    std::cout<<"\nAi castigat!\n";
                else
                    std::cout<<"\nAi pierdut! Incearca din nou!\n";
                getch();
            }
            else{
                if(snake.IsDead()){
                    std::string playerName;
                    std::cout<<"\nFelicitari pentru scorul obtinut: "<<snake.score<<'\n';
                    std::cout<<"Scrieti numele aici(intre 3 si 9 caractere): ";
                    std::cin>>playerName;
                    while(playerName.length() < 3 || playerName.length() > 9){
                        std::cout<<"Scrieti numele aici(intre 3 si 9 caractere): ";
                        std::cin>>playerName;
                    }
                    WriteScores(scores, playerName, snake.score);
                }
            }

            SDL_Quit();
        }

        else if(userInput == 3){
            std::string name;
            int score;
            system("cls");
            for(int i = 0; i < SCORES_SHOWN; i++){
                if(scores[i].name == "" || scores[i].score == -1)
                    std::cout<<std::setw(2)<<i + 1<<". "<<std::setw(10)<<"----------"<<"     "<<std::setw(10)<<"----------"<<'\n';
                else{
                    std::cout<<std::setw(2)<<i + 1<<". "<<std::setw(10)<<scores[i].name<<"     "<<std::setw(10)<<scores[i].score<<'\n';
                }
            }
            getch();
        }
        else if(userInput == -1)
            quitMenu = true;

        system("cls");
    }

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

void ReadScores(Score scores[SCORES_SHOWN])
{
    scoreFile.open("scores.txt", std::fstream::in);
    for(int i = 0; i < SCORES_SHOWN; i++){
        if(!(scoreFile>>scores[i].name))
            scores[i].name = "";
        if(!(scoreFile>>scores[i].score))
            scores[i].score = -1;
    }
    scoreFile.close();
}

void WriteScores(Score scores[SCORES_SHOWN], std::string nameToAdd, int scoreToAdd)
{
    int i;
    for(i = 0; i < SCORES_SHOWN && scores[i].name != "" && scores[i].score != -1; i++)
        if(scoreToAdd > scores[i].score)
            break;

    int j = 0;
    while(j < i-1){
        scores[j] = scores[j+1];
        j++;
    }
    scores[i].name = nameToAdd;
    scores[i].score = scoreToAdd;

    scoreFile.open("scores.txt", std::fstream::out | std::fstream::trunc);
    for(int i = 0; i < SCORES_SHOWN; i++){
        if(scores[i].name == "" || scores[i].score == -1)
            break;
        scoreFile<<scores[i].name<<'\n';
        scoreFile<<scores[i].score<<'\n';
    }
    scoreFile.close();
}
