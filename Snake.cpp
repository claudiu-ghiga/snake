#include "constants.h"
#include "Snake.h"
#include <iostream>



Snake::Snake(unsigned long headColor, unsigned long segmentColor)
{
    this->headColor = headColor;
    this->segmentColor = segmentColor;
    Snake::Restart();
}

Snake::~Snake()
{
    segments.clear();
}

void Snake::Restart()
{
    segments.clear();
    Snake::AddSegment(START_X, START_Y);
    direction = GO_NONE;
    directionBeforeUpdate = direction;
    segmentsToAdd = START_LENGTH;
    dead = false;
    speedModified = false;
    sizeModified = false;
    scoreModified = false;
    powerupTimeCounter = SDL_GetTicks();
    sizeRenderStartTime = SDL_GetTicks();
    speed = 1;
    score = 0;
}

void Snake::Update(Apple *apple, Powerup *powerup, bool gameMap[CELL_HEIGHT][CELL_WIDTH])
{
    if(dead){
        if(!(powerup->powerupDeactivated))
            powerup->Despawn();
        return;
    }

    if(timer.GetTicks() <  (SPEED_MULTIPLIER * 1000)/(FRAMES_PER_SECOND * speed))
        return;

    Snake::UpdateControls();

    timer.Start();

    directionBeforeUpdate = direction;

    if(Snake::CheckSelfCollision()){
        dead = true;
        return;
    }

    if(Snake::CheckAppleCollision(apple)){
        apple->Respawn(gameMap);
        score += 100;
        std::cout<<score<<std::endl;
    }
    else{
        if(direction != GO_NONE && (segments.size() > 1))
            segments.pop_back();
    }

    if(Snake::CheckPowerupCollision(powerup)){
        powerup->Despawn();
        Snake::UsePowerup(powerup);
    }
    if(!dead){
        if(powerup->powerupDeactivated && SDL_GetTicks() - powerup->time > powerup->timeout)
            powerup->Respawn();
    }
    else
        powerup->Despawn();

    UpdateSnakeModifiers(powerup);

    int oldX = segments.back().x;
    int oldY = segments.back().y;

    Snake::MoveSnake();

    if(direction != GO_NONE && segmentsToAdd > 0){
        segmentsToAdd--;
        if(segments.size() <= START_LENGTH)
            Snake::AddSegment(oldX, oldY);
    }
}

bool Snake::IsDead()
{
    return dead;
}

bool Snake::IsActive()
{
    return (direction != GO_NONE);
}

void Snake::Render(SDL_Renderer *renderer, SDL_Surface *surface)
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

    if(sizeModified){
        if(segments.size() > SEGMENTS_TO_REMOVE){
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            int length = segments.size();
                for(int i = length - 1; i > length - SEGMENTS_TO_REMOVE - 1; i--){
                    renderRect.x = segments[i].x * CELL_SIZE;
                    renderRect.y = segments[i].y * CELL_SIZE;
                    SDL_RenderFillRect(renderer, &renderRect);
                }
        }
    }
}

void Snake::UpdateControls()
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

void Snake::UpdateSnakeModifiers(Powerup *powerup)
{
    if(speedModified){
        if(speed == NORMAL_SPEED){
            speed = MOD_SPEED;
            powerupTimeCounter = SDL_GetTicks();
        }
        else if(SDL_GetTicks() - powerupTimeCounter > SPEED_POWERUP_TIME){
            speed = NORMAL_SPEED;
            speedModified = false;
        }
    }

    if(sizeModified)
        if(SDL_GetTicks() - sizeRenderStartTime > SIZE_RENDER_TIME){
            sizeModified = false;
            for(int i = 0; i < SEGMENTS_TO_REMOVE; i++)
                segments.pop_back();

        }
}

void Snake::AddSegment(unsigned int x, unsigned int y)
{
    SnakeSegment segment(x, y);
    segments.push_back(segment);
}

void Snake::MoveSnake()
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

bool Snake::CheckAppleCollision(Apple *apple)
{
    return (segments[0].x == apple->x && segments[0].y == apple->y);
}

bool Snake::CheckPowerupCollision(Powerup *powerup)
{
    return segments[0].x == powerup->x && segments[0].y == powerup->y;
}

bool Snake::CheckSelfCollision()
{
    unsigned int headX = segments[0].x;
    unsigned int headY = segments[0].y;

    if(segments.size() > 1)
        for(unsigned int i = 1; i < segments.size(); i++)
            if(segments[i].x == headX && segments[i].y == headY)
                return true;
    return false;
}

void Snake::UsePowerup(Powerup *powerup)
{
    unsigned int powerupType = powerup->getType();

    if(powerupType == SPEED)
        speedModified = true;

    if(powerupType == REDUCE_SIZE){
        if(segments.size() > SEGMENTS_TO_REMOVE)
            sizeModified = true;
            sizeRenderStartTime = SDL_GetTicks();
    }

    if(powerupType == BONUS){
        scoreModified = true;
        score += 300;
        std::cout<<score<<std::endl;
    }
}

