#pragma once
#include <iostream>
#include <vector>
#include "utils.h"
#include "Constants.h"

class Switch;
class FailMonster;
class Player;
class Point;

class Board {
private:
	char map[HEIGHT][WIDTH];// array representing the board
	mutable char lastBuffer[HEIGHT][WIDTH];
	mutable int lastColors[HEIGHT][WIDTH];

	bool hasDarkRoom = false;

	int darkRoomStartX = 0;
	int darkRoomEndX = 0;
	int darkRoomStartY = 0;
	int darkRoomEndY = 0;

	std::vector<Point> getObstacleCluster(int x, int y)const;

	bool checkLineOfSight(int x1, int y1, int x2, int y2) const;
public:
	Board();
	void resetMap();
	void setDarkRoom(bool isDark, int startX = 0, int endX = 0, int startY = 0, int endY = 0) {
		hasDarkRoom = true;
		darkRoomStartX = startX;
		darkRoomEndX = endX;
		darkRoomStartY = startY;
		darkRoomEndY = endY;
	}

	void addObject(int x, int y, char ch, int length = 1);
	void removeObject(int x, int y);
	void forceRefresh();

	char getCharAt(int x, int y) const;// get character at position (x, y)
	void print(const std::vector<Player>& players, std::vector<FailMonster>& monsters, const std::vector<Switch>& switches) const;

	bool isLit(int x, int y, const std::vector<Player>& players) const;
	bool isInsideDarkRoom(int x, int y) const;

	bool tryPushObstacle(int x, int y, int dirX, int dirY, int force, const std::vector<Player>& players);

	// check if a character is walkable	//its static so it can be used from outside without an instance of Board
	static bool isWalkable(char c) { return strchr(WALKABLE_CHARS, c) != nullptr; }
};