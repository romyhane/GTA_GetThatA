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

class Switch {
private:
	Point position;
	Point doorPosition;
	char doorChar;
	bool isOn;
	bool isLatching;

public:
	Switch();
	Switch(int x, int y) :position(x, y), doorPosition(0, 0), doorChar(SPACE), isOn(false), isLatching(false) {}

	void init(int x, int y, bool isLatching);

	void connectToDoor(Point doorPos, char doorChar);
	void checkPressure(Board& board, const std::vector<Player>& players, GameUI& ui);

	bool isRegular() const { return isLatching; }
	int getX() const { return position.getX(); }
	int getY() const { return position.getY(); }

	char getShape() const;
	void draw(const std::vector<Player>& players) const;
};