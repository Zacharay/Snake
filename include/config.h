#pragma once

#define STATUS_BAR_HEIGHT 80
#define TILE_SIZE 30

#define INITIAL_SNAKE_POS_X 5
#define INITIAL_SNAKE_POS_Y 5

#define SHORTEN_BONUS_FRAGMENTS 8
#define RED_DOT_LIFETIME 5 //in seconds

#define DELAY_AFTER_DEATH 2000 //in miliseconds
#define PAIRS_OF_PORTALS 2

struct GameConfig{
    int boardWidth;
    int boardHeight;
    float initialSpeed;
    int initialSnakeLength;
    int bonusFrequency;
    int pointsPerRedDot;
    int pointsPerBlueDot;
    float speedUpFactor;
    float slowDownFactor;

    GameConfig() {
        //default values
        boardWidth = 15;
        boardHeight = 20;
        initialSpeed = 0.5f;
        initialSnakeLength = 3;
        bonusFrequency = 5;
        pointsPerRedDot = 10;
        pointsPerBlueDot = 20;
        speedUpFactor = 1.1f;
        slowDownFactor = 0.9f;
    }
};