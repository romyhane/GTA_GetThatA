#pragma once
#include "Point.h"
#include "Constants.h"

class Board;

class FailMonster {
private:
    Point pos;
    int dirX;

public:
    FailMonster(int x, int y) : pos(x, y), dirX(1) {}

    int getX() const { return pos.getX(); }
    int getY() const { return pos.getY(); }
    char getShape() const { return FAIL_MONSTER; }

    void move(const Board& board);
};