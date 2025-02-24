#include "utilities.h""


iVec2 generateRandomCell(Board *board) {
    int posX, posY;
    int boardWidth = board->getBoardWidth();
    int boardHeight = board->getBoardHeight();
    do {

        posX = rand() % boardWidth ;
        posY = rand() % boardHeight;
    } while (
        *board->getCell(posX, posY) != BoardState::EMPTY ||
        posX == 1 ||
        posY == 1 ||
        posX == boardWidth - 2 ||
        posY == boardHeight - 2
    );
    return {posX, posY};
}
void LinkedList::insertAtHead(Node *newNode)
{
    Node *head = getHead();
    head->next = newNode;
}
Node* LinkedList::getHead()const {
    Node *temp = m_tail;

    while(temp->next != nullptr)
    {
        temp = temp->next;
    }

    return temp;
}
void LinkedList::removeTail()
{
    //prevent from shortening below 2 fragments
    if(m_tail->next == getHead()) {
        return;
    }
    Node *temp = m_tail;
    m_tail = m_tail->next;

    delete temp;
}
int LinkedList::getListLength()const {
    int length =1;
    Node *temp = m_tail;
    while(temp->next != nullptr) {
        temp = temp->next;
        length++;
    }
    return length;
}
LinkedList::~LinkedList(){

    Node *temp = m_tail;
    while (temp != nullptr) {
        Node *nextNode = temp->next;
        delete temp;
        temp = nextNode;
    }
}
iVec2 LinkedList::getTailPos() const{
    return m_tail->pos;
}