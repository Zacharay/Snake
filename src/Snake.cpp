#include "Snake.h"

Snake::Snake(iVec2 headPos,iVec2 tailPos,iVec2 initialMoveDirection)
{
    Node *tailNode = new Node(tailPos);
    Node *headNode = new Node(headPos);


    m_snakeList = new LinkedList(tailNode);
    m_snakeList->insertAtHead(headNode);
    m_moveDirection = initialMoveDirection;
    m_headPos = headPos;
    m_directionChanged = false;
}
void Snake::checkBorderCollisions(int boardWidth,int boardHeight) {

    if (m_moveDirection.x != 0 ) {
        if(m_headPos.x + m_moveDirection.x == 0 ||
            m_headPos.x + m_moveDirection.x == boardWidth - 1)
            {
            //make sure that snakes turns into valid direction when on first row
            if(m_headPos.y == 1) {
                m_moveDirection = iVec2(0,1);
            }
            else {
                m_moveDirection = iVec2(0,-1);
            }
        }
    }
    else if(m_moveDirection.y != 0 ) {
        if(m_headPos.y + m_moveDirection.y == 0 ||
            m_headPos.y + m_moveDirection.y == boardHeight - 1)
        {
            //make sure that snakes turns into valid direction when on first column
            if(m_headPos.x == boardWidth - 2) {
                m_moveDirection = iVec2(-1,0);
            }
            else {
                m_moveDirection = iVec2(1,0);
            }
        }
    }

}
void Snake::shortenSnake(int numberOfFragments) {
    for(int i=0;i<numberOfFragments;i++) {
        m_snakeList->removeTail();
    }
}
void Snake::move(iVec2 newHeadPos,bool doGrow)
{
    Node *newNode = new Node(newHeadPos);
    m_headPos = newHeadPos;
	m_snakeList->insertAtHead(newNode);
    if(!doGrow) {
        m_snakeList->removeTail();
    }

}
bool Snake::isCellAlreadyInList(iVec2 posToCheck) const{
    Node *node = m_snakeList->m_tail;
    while(node != nullptr) {
        if(node->pos.x == posToCheck.x && node->pos.y == posToCheck.y) {
            return true;
        }
        node = node->next;
    }
    return false;

}
void Snake::changeDirection(iVec2 direction)
{
        //prevent changing direction multiple times per game tick
        if(m_directionChanged) {
            return;
        }

        //cant turn right when going left and cant turn bottom when going up
        if(direction.x== -m_moveDirection.x || direction.y== -m_moveDirection.y) {
            return;
        }
        m_moveDirection = direction;
        m_directionChanged = true;
}
void Snake::setMoveDirection(iVec2 moveDirection) {
    m_moveDirection = moveDirection;
}

iVec2 Snake::getHeadPos()const {
    return m_headPos;
}
iVec2 Snake::getMoveDirection()const {
    return m_moveDirection;
}
void Snake::resetDirectionChanged() {
    m_directionChanged = false;
}
Snake::~Snake()
{
    delete m_snakeList;
}