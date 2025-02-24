#include "Game.h"
#include "utilities.h"
#include <cstdlib>
#include "string.h"

Game::Game()
    :Window(),
    InitialHeadPos(5,5),InitialTailPos(5,6),InitialMoveDirection(1,0)
{
    m_redDot = nullptr;
    m_board = new Board(m_config.boardWidth,m_config.boardHeight);
    initSnake();
    spawnNewBlueDot();
    m_gameSpeed = m_config.initialSpeed;
    m_points = 0.0f;
    m_speedUpTimer = 0.0f;
}
void Game::initSnake() {
    m_snake = new Snake(InitialHeadPos,InitialTailPos,InitialMoveDirection);
    for(int i=0;i<m_config.initialSnakeLength-2;i++) {
        m_snake->checkBorderCollisions(m_config.boardWidth,m_config.boardHeight);
        iVec2 newHeadPos = m_snake->getHeadPos() + m_snake->getMoveDirection();
        m_snake->move(newHeadPos,true);
    }
}
void Game::onUpdate()
{
    if(m_timer > m_gameSpeed)
    {
        if(m_redDot != nullptr) {
            m_redDot->timeLeft -= m_timer;

            if(m_redDot->timeLeft <= 0) {
                m_board->setCell(m_redDot->pos.x,m_redDot->pos.y,BoardState::EMPTY);
                delete m_redDot;
                m_redDot = nullptr;
            }
        }

        m_speedUpTimer += m_timer;
        moveSnake();
        m_timer = 0;
    }

    if(m_speedUpTimer > SpeedUpCooldown) {
        m_speedUpTimer = 0;
        m_gameSpeed *= m_config.speedUpFactor;

        if(m_gameSpeed < MaxGameSpeed) {
            m_gameSpeed = MaxGameSpeed ;
        }

    }

}
void Game::moveSnake() {
    m_snake->checkBorderCollisions(m_config.boardWidth,m_config.boardHeight);
    iVec2 newHeadPos = m_snake->getHeadPos() + m_snake->getMoveDirection();

    if(m_snake->isCellAlreadyInList(newHeadPos)) {
        endGame();
        return;
    }

    bool doGrow = false;
    if(*m_board->getCell(newHeadPos.x,newHeadPos.y) == BoardState::R_DOT) {
        m_points += m_config.pointsPerRedDot;

        if(m_redDot->typeOfBonus == RD_SLOW_BONUS) {
            m_gameSpeed *= m_config.slowDownFactor;
        }
        else {
            m_snake->shortenSnake(SHORTEN_BONUS_FRAGMENTS);
        }

        m_board->setCell(newHeadPos.x,newHeadPos.y,BoardState::EMPTY);
        delete m_redDot;
        m_redDot = nullptr;
    }
    else if(*m_board->getCell(newHeadPos.x,newHeadPos.y)== BoardState::B_DOT) {
        spawnNewBlueDot();
        m_points += m_config.pointsPerBlueDot;
        doGrow = true;
        m_board->setCell(newHeadPos.x,newHeadPos.y,BoardState::EMPTY);
        trySpawningRedDot();
    }
    else if(*m_board->getCell(newHeadPos.x,newHeadPos.y) < BoardState::Portal) {
        iVec2 secondPortalPos = m_board->findSecondPortalPos(newHeadPos);
        iVec2 teleportedHeadPos = secondPortalPos + m_snake->getMoveDirection();

        m_snake->move(teleportedHeadPos,false);
        return;
    }
    m_snake->move(newHeadPos,doGrow);

    // Unlock direction change after moving
    m_snake->resetDirectionChanged();
}
void Game::spawnNewBlueDot() {
    iVec2 randomEmptyCell = generateRandomCell(m_board);
    m_board->setCell(randomEmptyCell.x,randomEmptyCell.y,BoardState::B_DOT);
}
void Game::trySpawningRedDot() {
    int val = rand() % 100;
    if(val < m_config.bonusFrequency && m_redDot == nullptr) {
        iVec2 randomEmptyCell = generateRandomCell(m_board);
        m_board->setCell(randomEmptyCell.x,randomEmptyCell.y,BoardState::R_DOT);

        int typeOfBonus = rand()%2;
        m_redDot = new RedDot(randomEmptyCell,RED_DOT_LIFETIME,typeOfBonus);
    }

}
void Game::resetGame() {
    delete m_snake;
    initSnake();
    m_board->resetBoard();
    spawnNewBlueDot();
    m_points = 0;
    m_gameSpeed = m_config.initialSpeed;
    m_elapsedTime = 0.0f;
    m_speedUpTimer = 0.0f;
    m_timer = 0;
    m_redDot = nullptr;
    m_gameOver = false;
    m_activePlayerTextInput = false;
    m_displayHighScoreScreen = false;

    memset(m_currentPlayerInput,0,MAX_NICKNAME_LENGTH);
    std::cout<<m_currentPlayerInput<<std::endl;
    m_currentInputLength = 0;
    m_currentScore = 0;
}
void Game::endGame() {
    if(m_highScoreManager.qualifiesForHighScore(m_points)) {
        m_activePlayerTextInput = true;
        SDL_StartTextInput();
        m_currentScore = m_points;
    }

    m_gameOver = true;
    m_displayHighScoreScreen = true;


    SDL_Delay(DELAY_AFTER_DEATH);
}
void Game::saveGame() {
    const char* filename = "save.dat";
    FILE* file = fopen(filename, "wb");
    if (!file) {
        printf("Failed to open save file");
        return;
    }

    // Save simple members
    fwrite(&m_points, sizeof(m_points), 1, file);
    fwrite(&m_gameSpeed, sizeof(m_gameSpeed), 1, file);
    fwrite(&m_elapsedTime, sizeof(m_elapsedTime), 1, file);

    // Save the board dimensions
    int boardWidth = m_board->getBoardWidth();
    int boardHeight = m_board->getBoardHeight();
    fwrite(&boardWidth, sizeof(boardWidth), 1, file);
    fwrite(&boardHeight, sizeof(boardHeight), 1, file);

    // Save the board content
    int** boardArray = m_board->getBoard();
    for (int y = 0; y < boardHeight; ++y) {
        fwrite(boardArray[y], sizeof(int), boardWidth, file);
    }

    // Save the red dot if it exists
    bool hasRedDot = (m_redDot != nullptr);
    fwrite(&hasRedDot, sizeof(hasRedDot), 1, file);
    if (hasRedDot) {
        fwrite(&m_redDot->pos, sizeof(m_redDot->pos), 1, file);
        fwrite(&m_redDot->timeLeft, sizeof(m_redDot->timeLeft), 1, file);
        fwrite(&m_redDot->typeOfBonus, sizeof(m_redDot->typeOfBonus), 1, file);
    }

    int snakeLength = m_snake->m_snakeList->getListLength();
    fwrite(&snakeLength,sizeof(int),1,file);

    iVec2 moveDirection = m_snake->getMoveDirection();
    fwrite(&moveDirection,sizeof(iVec2),1,file);

    //save snake fragments positions
    Node *current = m_snake->m_snakeList->m_tail;
    while (current->next != nullptr ) {
        iVec2 position = current->pos;
        fwrite(&position, sizeof(iVec2), 1, file);
        current = current->next;
    }
    iVec2 position = current->pos;
    fwrite(&position, sizeof(iVec2), 1, file);

    fclose(file);
}
void Game::loadGame() {
    const char* filename = "save.dat";
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("No save file found");
        return;
    }

    // Load simple members
    fread(&m_points, sizeof(m_points), 1, file);
    fread(&m_gameSpeed, sizeof(m_gameSpeed), 1, file);
    fread(&m_elapsedTime, sizeof(m_elapsedTime), 1, file);

    // Load the board dimensions
    int boardWidth, boardHeight;
    fread(&boardWidth, sizeof(boardWidth), 1, file);
    fread(&boardHeight, sizeof(boardHeight), 1, file);
    if(boardHeight != m_board->getBoardHeight() || boardWidth != m_board->getBoardWidth()) {
        printf("Board size mismatch");
        fclose(file);
        return;
    }


    // Recreate the board and load its content
    delete m_board;
    m_board = new Board(boardWidth, boardHeight);
    int** boardArray = m_board->getBoard();
    for (int y = 0; y < boardHeight; ++y) {
        fread(boardArray[y], sizeof(int), boardWidth, file);
    }

    bool hasRedDot;
    fread(&hasRedDot, sizeof(hasRedDot), 1, file);
    if (hasRedDot) {
        m_redDot = new RedDot();
        fread(&m_redDot->pos, sizeof(m_redDot->pos), 1, file);
        fread(&m_redDot->timeLeft, sizeof(m_redDot->timeLeft), 1, file);
        fread(&m_redDot->typeOfBonus, sizeof(m_redDot->typeOfBonus), 1, file);
    } else {
        m_redDot = nullptr;
    }
    delete m_snake;

    int snakeLength;
    fread(&snakeLength, sizeof(snakeLength), 1, file);

    iVec2 moveDirection;
    fread(&moveDirection, sizeof(iVec2), 1, file);


    iVec2 headPos,tailPos;
    fread(&tailPos, sizeof(iVec2), 1, file);
    fread(&headPos, sizeof(iVec2), 1, file);
    m_snake = new Snake(headPos,tailPos,iVec2(1,0));

    for (int i = 0; i < snakeLength-2; ++i) {
        iVec2 position;
        fread(&position, sizeof(iVec2), 1, file);
        m_snake->move(position, true);
    }
    m_snake->setMoveDirection(moveDirection);

    fclose(file);
}
void Game::drawInterface() {
    char timeText[128];
    char pointsText[128];
    const char *requirements = "Requirements: 1234ABCDEFGH";
    snprintf(timeText, sizeof(timeText), "Time: %.2f", m_elapsedTime);
    snprintf(pointsText, sizeof(pointsText), "Points: %d", m_points);

    drawString(10, 10, "Zachariasz Slupski 203514");
    drawString(10, 30 ,requirements);
    drawString(500,30, pointsText);
    drawString(500, 10, timeText);
    drawString(10,50,"'n' - new game");
    drawString(300,50,"'Esc' - exit the program");
    if(m_redDot != nullptr) {
        drawRedDotBar();
    }

}
void Game::onRender()
{
    int** boardArray = m_board->getBoard();
    int boardHeight = m_config.boardHeight;
    int boardWidth = m_config.boardWidth;

    for(int y = 0; y < boardHeight; y++)
        for(int x = 0; x < boardWidth; x++)
        {
            if(boardArray[y][x] == BoardState::EMPTY) {
                continue;
            }
            int tileX,tileY;
            tileX = x * TILE_SIZE;
            tileY = y * TILE_SIZE + STATUS_BAR_HEIGHT;

            if(boardArray[y][x] == BoardState::R_DOT){
                drawDots(tileX,tileY,Texture::TEX_RED_DOT);
            }
            else if(boardArray[y][x] == BoardState::B_DOT){
                drawDots(tileX,tileY,Texture::TEX_BLUE_DOT);
            }
            else if(boardArray[y][x]== BoardState::BRICK) {
                drawStatic(tileX,tileY,Texture::TEX_BRICK);
            }
            else if(boardArray[y][x] < BoardState::Portal) {
                drawStatic(tileX,tileY,Texture::TEX_PORTAL);

                int numberIndex  = -boardArray[y][x];

                char text[2];
                snprintf(text, sizeof(text), "%d", numberIndex);
                const int numberOffsetX = 10;
                const int numberOffsetY = 8;
                drawString(tileX+numberOffsetX,tileY+numberOffsetY,text);
            }
        }

    drawSnake();
}
void Game::drawRedDotBar() {
    SDL_Rect blueRect = { 700, 10, 150, 30 };  // x, y, width, height
    int blueColor = SDL_MapRGB(m_screen->format, 0x00, 0x00, 0xFF); // Blue color
    SDL_FillRect(m_screen, &blueRect, blueColor);

    float percentageFill = m_redDot->timeLeft/5.0f ;
    int redRectWidth = (blueRect.w - 10) * percentageFill;
    SDL_Rect redRect = { blueRect.x + 5, blueRect.y + 5,redRectWidth , blueRect.h - 10 };
    int redColor = SDL_MapRGB(m_screen->format, 0xFF, 0x00, 0x00); // Red color
    SDL_FillRect(m_screen, &redRect, redColor);
}
void Game::drawSnake() {
    Node *temp = m_snake->m_snakeList->m_tail;

    int x = temp->pos.x,y = temp->pos.y;
    int tileX = x * TILE_SIZE;
    int tileY = y * TILE_SIZE + STATUS_BAR_HEIGHT;

    drawStatic(tileX,tileY,Texture::TEX_SNAKE_TAIL);

    int fragmentNumber=1;
    while(temp->next != nullptr)
    {
        temp = temp->next;
        int x = temp->pos.x,y = temp->pos.y;
        int tileX = x * TILE_SIZE;
        int tileY = y * TILE_SIZE + STATUS_BAR_HEIGHT;
        if(fragmentNumber%2==0) {
            drawStatic(tileX,tileY,Texture::TEX_SNAKE_BODY,0.75);
        }
        else {
            drawStatic(tileX,tileY,Texture::TEX_SNAKE_BODY,1.0f);
        }
        fragmentNumber++;
    }
    x = temp->pos.x,y = temp->pos.y;
    tileX = x * TILE_SIZE;
    tileY = y * TILE_SIZE + STATUS_BAR_HEIGHT;
    drawStatic(tileX,tileY,Texture::TEX_SNAKE_HEAD);
}
void Game::processEvents()
{
        SDL_Event event;
        // Event handling
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                m_quit = true;
            }
            else if(event.type == SDL_KEYDOWN)
            {
                    switch(event.key.keysym.sym)
                    {
                        case SDLK_UP :
                            m_snake->changeDirection(iVec2(0,-1));
                        break;
                        case SDLK_DOWN:
                            m_snake->changeDirection(iVec2(0,1));
                        break;
                        case SDLK_LEFT :
                            m_snake->changeDirection(iVec2(-1,0));
                        break;
                        case SDLK_RIGHT:
                            m_snake->changeDirection(iVec2(1,0));
                        break;
                        case SDLK_ESCAPE:
                            m_quit = true;
                        break;
                        case SDLK_n:
                            resetGame();
                        break;
                        case SDLK_s:
                            saveGame();
                        break;
                        case SDLK_l:
                            loadGame();
                        break;
                   }
            }
        }
}
Game::~Game() {
    delete m_snake;
    delete m_board;
    delete m_redDot;
}