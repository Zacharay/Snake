#pragma once
#include "utilities.h"

enum MoveDirection{
    RIGHT,
    LEFT,
    TOP,
    BOTTOM
};

class Snake{
    iVec2 m_headPos;
    iVec2 m_moveDirection;
    bool m_directionChanged;

    public:
    LinkedList *m_snakeList;
    Snake(iVec2 headPos,iVec2 tailPos,iVec2 initialMoveDirection);
    void move(iVec2 newHeadPos,bool doGrow);
    void changeDirection(iVec2 direction);
    void checkBorderCollisions(int boardWidth,int boardHeight);
    void shortenSnake(int numberOfFragments);
    bool isCellAlreadyInList(iVec2 posToCheck)const;
    void setMoveDirection(iVec2 moveDirection);
    iVec2 getHeadPos()const;
    iVec2 getMoveDirection()const;
    void resetDirectionChanged();
    ~Snake();
};