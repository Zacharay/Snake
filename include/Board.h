#pragma once
#include "utilities.h"

typedef struct iVec2;

enum BoardState{
    Portal,
    EMPTY,
    BRICK,
    R_DOT,
    B_DOT,
    SNAKE_HEAD,
    SNAKE_BODY_LARGE,
    SNAKE_BODY_SMALL,
    SNAKE_TAIL
};

class Board {
private:
    const int m_boardWidth;
    const int m_boardHeight;
    int **board;
public:
    Board(int width,int height);
    int **getBoard();
    void resetBoard();
    int* getCell(int x,int y);
    void setCell(int x,int y,int boardState);
    iVec2 findSecondPortalPos(iVec2 firstPortalPos)const ;
    int getBoardWidth()const;
    int getBoardHeight()const;
};