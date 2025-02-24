#pragma once
#include "Window.h"
#include "Snake.h"

#define RD_SLOW_BONUS 0
#define RD_SHORTER_BONUS 1


struct RedDot {
    iVec2 pos;
    float timeLeft;
    int typeOfBonus;
    RedDot(iVec2 pos,float timeLeft , int typeOfBonus) {
        this->pos = pos;
        this->timeLeft = timeLeft;
        this->typeOfBonus = typeOfBonus;
    }
    RedDot() {
        this->pos = iVec2(0, 0);
        this->timeLeft = 0;
        this->typeOfBonus = 0;
    }
};



class Game : public Window {
private:
    const iVec2 InitialHeadPos;
    const iVec2 InitialTailPos;
    const iVec2 InitialMoveDirection;
    const float MaxGameSpeed = 0.04f;
    const float SpeedUpCooldown = 10.0f;

    Board *m_board;
    RedDot *m_redDot;
    Snake *m_snake;
    int m_points;
    float m_gameSpeed;
    float m_speedUpTimer;

    void initSnake();
    void moveSnake();
    void spawnNewBlueDot();
    void trySpawningRedDot();

    void saveGame();
    void loadGame();
    void endGame();
    void resetGame();

    void drawSnake();
    void drawRedDotBar();
public:
    Game();
    ~Game();
protected:
    void processEvents()override;
    void onUpdate() override;
    void onRender() override;
    void drawInterface() override;
};