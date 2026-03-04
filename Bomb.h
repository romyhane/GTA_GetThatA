#pragma once
#include <string>
#include <iostream>
#include <vector>
#include "Point.h"
#include "utils.h"
#include "Constants.h"
#include "Board.h"
#include "Player.h"
#include "GameUI.h"

class Game;

class Bomb {
private:
	Point position;
	bool isActive;
	int timer;

	void explode(Board& board);
public:
	Bomb();

	void activate(int x, int y);
	bool getIsActive() const { return isActive; }
	bool tick(Board& board, std::vector<Player>& players, GameUI& ui, bool& gameOver, Game* game);

	char getShape() const { return isActive ? BOMB : SPACE; }
	int getX() const { return position.getX(); }
	int getY() const { return position.getY(); }
};
