#include "Board.h"

Board::Board(int width,int height)
    :m_boardWidth(width),m_boardHeight(height)
{
    board = new int*[m_boardHeight];
    for(int i=0;i<m_boardHeight;i++) {
        board[i] = new int[m_boardWidth];
    }


    for(int x=0;x<m_boardWidth;x++)
    {
        for(int y=0;y<m_boardHeight;y++)
        {
            board[y][x] = BoardState::EMPTY;
        }
    }

    //init borders
    for(int x=0;x<m_boardWidth;x++) {
        board[0][x] = BoardState::BRICK;
        board[m_boardHeight-1][x] = BoardState::BRICK;
    }
    for(int y=0;y<m_boardHeight;y++) {
        board[y][0] = BoardState::BRICK;
        board[y][m_boardWidth-1] = BoardState::BRICK;
    }

    for(int i=1;i<=PAIRS_OF_PORTALS;i++) {
        iVec2 firstPortalCoords = generateRandomCell(this);
        iVec2 secondPortalCoords = generateRandomCell(this);

       board[firstPortalCoords.y][firstPortalCoords.x] = -i;
       board[secondPortalCoords.y][secondPortalCoords.x] = -i;
    }


}
void Board::resetBoard() {
    for(int x=0;x<m_boardWidth;x++)
    {
        for(int y=0;y<m_boardHeight;y++)
        {
            if(board[y][x]!= BoardState::BRICK) {
                board[y][x]= BoardState::EMPTY;
            }
        }
    }

    for(int i=1;i<=PAIRS_OF_PORTALS;i++) {
        iVec2 firstPortalCoords = generateRandomCell(this);
        iVec2 secondPortalCoords = generateRandomCell(this);

        board[firstPortalCoords.y][firstPortalCoords.x] = -i;
        board[secondPortalCoords.y][secondPortalCoords.x] = -i;
    }
}
void Board::setCell(int x,int y,int boardState) {
    board[y][x]=boardState;
}
int **Board::getBoard(){
    return board;
}
int* Board::getCell(int x,int y)
{
    if(y<0||x<0||x>=m_boardWidth || y>=m_boardHeight)
    {
       printf("Snake wyszedl poza ekran");
       return nullptr;
    }
    return &board[y][x];
}
iVec2 Board::findSecondPortalPos(iVec2 firstPortalPos)const {
    int portalIndex = board[firstPortalPos.y][firstPortalPos.x];

    for(int y=0;y<m_boardHeight;y++)
        for(int x=0;x<m_boardWidth;x++) {
            if(firstPortalPos.y == y && firstPortalPos.x == x) {
                continue;
            }

            if(board[y][x] == portalIndex) {
                return {x,y};
            }
        }

    return {-1,-1};
}
int Board::getBoardHeight() const {
    return m_boardHeight;
}
int Board::getBoardWidth() const {
    return m_boardWidth;
}
