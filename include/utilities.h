#pragma once
#include <iostream>
#include "config.h"
#include "Board.h"



class Board;

typedef struct iVec2 {
    int x, y;

    iVec2() : x(0), y(0) {}
    iVec2(int xx, int yy) : x(xx), y(yy) {}

    iVec2 operator+(const iVec2& otherVector) const {
        return iVec2(x + otherVector.x, y + otherVector.y);
    }

    iVec2& operator=(const iVec2& other) {
        if (this == &other) {
            return *this;
        }
        x = other.x;
        y = other.y;
        return *this;
    }
} iVec2;

struct Node {
    iVec2 pos;
    Node *next;

    Node(iVec2 position) : pos(position), next(nullptr) {}

};


class LinkedList{

public:
    Node* m_tail;
    LinkedList(Node *startingNode):m_tail(startingNode){};
    void insertAtHead(Node *newNode);
    Node *getHead()const;
    void removeTail();
    int getListLength()const;
    iVec2 getTailPos()const;
    ~LinkedList();
};
 iVec2 generateRandomCell(Board *board);