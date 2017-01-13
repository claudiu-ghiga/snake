#include "constants.h"
#include "SnakeAI.h"
#include <iostream>

SnakeAI::SnakeAI(unsigned long headColor, unsigned long segmentColor, int userInput)
{
    this->headColor = headColor;
    this->segmentColor = segmentColor;
    dead = false;
    if(userInput == 2)
        SnakeAI::Restart();
}

SnakeAI::~SnakeAI()
{
    segments.clear();
}

void SnakeAI::Restart()
{
    segments.clear();
    SnakeAI::AddSegment(AI_START_X, AI_START_Y);
    direction = GO_NONE;
    segmentsToAdd = START_LENGTH;
    time = 0;
    timeout = 30;
    score = 0;
}

void SnakeAI::Update(Snake *playerSnake, Apple *apple, bool gameMap[CELL_HEIGHT][CELL_WIDTH])
{
    if(!(playerSnake->IsActive()) || playerSnake->IsDead() || dead)
        return;

    if(timer.GetTicks() < (SPEED_MULTIPLIER * 1000)/FRAMES_PER_SECOND)
        return;

    timer.Start();

    if(SnakeAI::CheckSelfCollision())
        dead = true;

    if(SnakeAI::CheckAppleCollision(apple)){
        apple->Respawn(gameMap);
        score += 100;
    }
    else{
        if(direction != GO_NONE && (segments.size() > 1))
            segments.pop_back();
    }
    if(apple->hasRespawned){
        dirs.clear();
        dirs = SnakeAI::ComputePath(apple);
        apple->hasRespawned = false;
    }
    direction = SnakeAI::ComputeDirection(dirs);


    int oldX = segments.back().x;
    int oldY = segments.back().y;

    SnakeAI::MoveSnake();

    if(direction != GO_NONE && segmentsToAdd > 0){
        segmentsToAdd--;
        if(segments.size() <= START_LENGTH)
            SnakeAI::AddSegment(oldX, oldY);
    }
}

bool SnakeAI::IsDead()
{
    return dead;
}

void SnakeAI::Render(SDL_Renderer *renderer, SDL_Surface *surface)
{
    renderRect.w = CELL_SIZE;
    renderRect.h = CELL_SIZE;
    renderRect.x = segments[0].x * CELL_SIZE;
    renderRect.y = segments[0].y * CELL_SIZE;

    Uint8 r = 0, g = 0, b = 0;

    SDL_GetRGB(headColor, surface->format, &r, &g, &b);
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);

    SDL_RenderFillRect(renderer, &renderRect);

    SDL_GetRGB(segmentColor, surface->format, &r, &g, &b);
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);

    for(unsigned int i = 1; i < segments.size(); i++){
        renderRect.x = segments[i].x * CELL_SIZE;
        renderRect.y = segments[i].y * CELL_SIZE;
        SDL_RenderFillRect(renderer, &renderRect);
    }
}

void SnakeAI::AddSegment(unsigned int x, unsigned int y)
{
    SnakeSegment segment(x, y);
    segments.push_back(segment);
}

int SnakeAI::ComputeDirection(std::deque<int> &directionsQueue)
{
    int nextDirection = 0;
    if(!(directionsQueue.empty())){
        nextDirection = directionsQueue.front();
        directionsQueue.pop_front();
    }
    else
        nextDirection = direction;

    return nextDirection;
}

std::deque<int> SnakeAI::ComputePath(Apple *apple)
{
    std::deque<int> directionQueue;
    directionQueue.clear();

    std::deque<Cell> cellQueue;
    cellQueue.clear();

    Cell gameBoard[CELL_HEIGHT][CELL_WIDTH];

    for(int i = 0; i < CELL_HEIGHT; i++)
        for(int j = 0; j < CELL_WIDTH; j++){
            gameBoard[i][j].x = j;
            gameBoard[i][j].y = i;
            gameBoard[i][j].parent = nullptr;
        }

    Cell start = gameBoard[segments[0].y][segments[0].x];
    gameBoard[start.y][start.x].parent = &(gameBoard[start.y][start.x]);
    start.parent = &start;
    cellQueue.push_back(start);

    static const int moveX[] = {0, 0, 0, -1, 1};
    static const int moveY[] = {0, -1, 1, 0, 0};
    int offsetX, offsetY;
    bool foundApple = false;
    Cell c;

    while(!cellQueue.empty() && !foundApple){
        c = cellQueue.front();
        if(c.x == apple->x && c.y == apple->y){
            foundApple = true;
            break;
        }
        cellQueue.pop_front();

        for(int i = 1; i <= 4; i++){
            offsetX = moveX[i];
            offsetY = moveY[i];
            int lineIndex = c.y + offsetY;
            int columnIndex = c.x + offsetX;

            if(lineIndex < 0)
                lineIndex = CELL_HEIGHT - 1;
            lineIndex %= CELL_HEIGHT;
            if(columnIndex < 0)
                columnIndex = CELL_WIDTH - 1;
            columnIndex %= CELL_WIDTH;

            if(SnakeAI::IsValidDir(gameBoard[lineIndex][columnIndex])){
                gameBoard[lineIndex][columnIndex].parent = &(gameBoard[c.y][c.x]);
                cellQueue.push_back(gameBoard[lineIndex][columnIndex]);
            }
        }
    }

    Cell *pathCell = &c;
    if(foundApple){
        while(pathCell->x != start.x || pathCell->y != start.y){
            int xDiff = pathCell->parent->x - pathCell->x;
            int yDiff = pathCell->parent->y - pathCell->y;
            if(xDiff == 0){
                if(yDiff == 1 || yDiff == -(CELL_HEIGHT - 1))
                    directionQueue.push_front(GO_UP);
                if(yDiff == -1 || yDiff == (CELL_HEIGHT - 1))
                    directionQueue.push_front(GO_DOWN);
            }
            else{
                if(xDiff == 1 || xDiff == -(CELL_WIDTH - 1))
                    directionQueue.push_front(GO_LEFT);
                if(xDiff == -1 || xDiff == (CELL_WIDTH - 1))
                    directionQueue.push_front(GO_RIGHT);
            }
            pathCell = pathCell->parent;
        }
    }

    return directionQueue;
}

bool SnakeAI::IsValidDir(Cell c)
{
    if(c.x >= CELL_WIDTH || c.x < 0 ||
       c.y >= CELL_HEIGHT || c.y < 0)
        return false;
    if(c.parent != nullptr)
        return false;
    int snakeLength = segments.size();
    for(int i = 1; i < snakeLength; i++)
        if(c.x == segments[i].x && c.y == segments[i].y)
            return false;
    return true;
}

void SnakeAI::MoveSnake()
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

bool SnakeAI::CheckAppleCollision(Apple *apple)
{
    return (segments[0].x == apple->x && segments[0].y == apple->y);
}

bool SnakeAI::CheckSelfCollision()
{
    unsigned int headX = segments[0].x;
    unsigned int headY = segments[0].y;

    if(segments.size() > 1)
        for(unsigned int i = 1; i < segments.size(); i++)
            if(segments[i].x == headX && segments[i].y == headY)
                return true;
    return false;
}
