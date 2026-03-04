#include "FailMonster.h"
#include "Board.h"

void FailMonster::move(const Board& board) {
    int nextX = pos.getX() + dirX;
    int y = pos.getY();
    char nextCell = board.getCharAt(nextX, y);

    if (!Board::isWalkable(nextCell) || nextCell == WALL_V || nextCell == CORNER) {
        dirX *= -1;
    }
    else {
        pos.set(nextX, y);
    }
}