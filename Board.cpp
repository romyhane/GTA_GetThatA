#include "Board.h"
#include "Player.h"
#include <cmath>
#include "FailMonster.h"
#include <queue>
#include <vector>
#include <algorithm>
#include "Switch.h"
#include "utils.h"


using std::cout;
using std::endl;
using std::vector;
using std::abs;

//constructor initializes the board by resetting the map
Board::Board() {
	resetMap();
	forceRefresh();
}

void Board::forceRefresh() {
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			lastBuffer[i][j] = 0;
			lastColors[i][j] = 0;
		}
	}
}

void Board::resetMap() {
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			// first, clean with spaces
			map[i][j] = SPACE;
		}
	}
}

void Board::addObject(int x, int y, char ch, int length) {
	for (int i = 0; i < length; i++) {
		int currentY = y + i;

		if (x >= 0 && x < WIDTH && currentY >= 0 && currentY < HEIGHT) {
			map[currentY][x] = ch;
			if (!isGlobalSilent()) {
				gotoxy(x, currentY);
				cout << ch;
			}
		}
	}
}

void Board::removeObject(int x, int y) {
	// check bounds
	if (x < LEFT_WALL_X || x > RIGHT_WALL_X || y < TOP_WALL_Y || y > BOTTOM_WALL_Y) return;

	char target = map[y][x];

	// object is a door
	if (target >= DOOR_NUMBER_1 && target <= DOOR_NUMBER_9) {

		//find the upper boundary of the door
		int startY = y;
		while (startY > TOP_WALL_Y && map[startY - 1][x] == target) {
			startY--;
		}

		//find the lower boundary of the door
		int endY = y;
		while (endY < BOTTOM_WALL_Y && map[endY + 1][x] == target) {
			endY++;
		}

		//delete the entire door
		for (int i = startY; i <= endY; i++) {
			map[i][x] = SPACE;
			if (!isGlobalSilent()) {
				gotoxy(x, i);
				cout << SPACE;
			}
		}
	}

	//object != door
	else {
		map[y][x] = SPACE;
		if (!isGlobalSilent()) {
		gotoxy(x, y);
		cout << SPACE;
		}
	}
}

char Board::getCharAt(int x, int y) const {
	return map[y][x];
}

//helper function to check if a point exists in a vector
bool isPointInList(const std::vector<Point>& list, int x, int y) {
	for (const auto& p : list) {
		if (p.getX() == x && p.getY() == y) return true;
	}
	return false;
}

//uses BFS (Breadth-First Search) to find all connected parts of an obstacle cluster
//gemini explained this algorithm to us
std::vector<Point> Board::getObstacleCluster(int startX, int startY) const {
	std::vector<Point> cluster;
	//if the starting point isn't an obstacle, return empty
	if (map[startY][startX] != OBSTACLE) return cluster;

	std::queue<Point> q;
	Point start(startX, startY);

	q.push(start);
	cluster.push_back(start);

	while (!q.empty()) {
		Point curr = q.front();
		q.pop();

		//directions: Down, Up, Right, Left
		int dx[] = { 0, 0, 1, -1 };
		int dy[] = { 1, -1, 0, 0 };

		for (int i = 0; i < NUM_DIRECTIONS; i++) {
			int nx = curr.getX() + dx[i];
			int ny = curr.getY() + dy[i];

			if (nx >= 0 && nx < WIDTH && ny >= 0 && ny < HEIGHT) {
				if (map[ny][nx] == OBSTACLE) {
					// if this neighbor is an obstacle and not yet visited, add to queue
					if (!isPointInList(cluster, nx, ny)) {
						Point nextP(nx, ny);
						cluster.push_back(nextP);
						q.push(nextP);
					}
				}
			}
		}
	}
	return cluster;
}

// main logic for pushing obstacles
bool Board::tryPushObstacle(int x, int y, int dirX, int dirY, int force, const std::vector<Player>& players) {
	if (dirX == 0 && dirY == 0) return false;

	//step 1: Identify the cluster
	std::vector<Point> cluster = getObstacleCluster(x, y);
	if (cluster.empty()) return false;

	//step 2: Check Force vs Mass
	if (force < (int)cluster.size()) { return false; }

	//step 3: Check Collision (Is the target area free?)
	for (const auto& p : cluster) {
		int targetX = p.getX() + dirX;
		int targetY = p.getY() + dirY;

		if (targetX <= LEFT_WALL_X || targetX >= RIGHT_WALL_X ||
			targetY <= TOP_WALL_Y || targetY >= BOTTOM_WALL_Y) {
			return false;
		}

		char targetCell = map[targetY][targetX];
		bool isSelf = isPointInList(cluster, targetX, targetY);

		if (!isSelf) {
			if (!Board::isWalkable(targetCell)) return false;
			for (const auto& player : players) {
				if (player.isActive() && player.getX() == targetX && player.getY() == targetY) return false;
			}
		}
	}

	//step 4: Perform the move
	for (const auto& p : cluster) {
		map[p.getY()][p.getX()] = SPACE;
		if (!isGlobalSilent()) {
			gotoxy(p.getX(), p.getY());
			cout << SPACE;
		}
	}


	if (!isGlobalSilent())	setTextColor(Color::YELLOW);
	for (const auto& p : cluster) {
		int newX = p.getX() + dirX;
		int newY = p.getY() + dirY;
		map[newY][newX] = OBSTACLE;
		if (!isGlobalSilent()) {
			gotoxy(newX, newY);
			cout << OBSTACLE;
		}
	}
	if (!isGlobalSilent()) setTextColor(Color::WHITE);

	return true;
}

//uses Bresenham's Line Algorithm to check visibility between two points
//gemini explained this algorithm to us
bool Board::checkLineOfSight(int x1, int y1, int x2, int y2) const {
	int dx = abs(x2 - x1);
	int sx = x1 < x2 ? DIR_POSITIVE : DIR_NEGATIVE;
	int dy = -abs(y2 - y1);
	int sy = y1 < y2 ? DIR_POSITIVE : DIR_NEGATIVE;
	int err = dx + dy, e2;

	while (true) {
		if (x1 == x2 && y1 == y2) return true;

		char c = map[y1][x1];
		bool blocksLight = (c == WALL_H || c == WALL_V || c == CORNER ||
			(c >= DOOR_NUMBER_1 && c <= DOOR_NUMBER_9));

		if (blocksLight) return false;

		if (e2 = BRESENHAM_MULTIPLIER * err; e2 >= dy) { err += dy; x1 += sx; }
		if (e2 <= dx) { err += dx; y1 += sy; }
	}
}

bool Board::isLit(int x, int y, const std::vector<Player>& players) const {
	for (const auto& p : players) {
		if (p.isActive() && p.hasTorch()) {
			if (abs(p.getX() - x) <= LIGHT_RADIUS && abs(p.getY() - y) <= LIGHT_RADIUS) {
				if (checkLineOfSight(p.getX(), p.getY(), x, y)) {
					return true;
				}
			}
		}
	}
	return false;
}

bool Board::isInsideDarkRoom(int x, int y) const {
	if (!hasDarkRoom) { return false; }
	return (x >= darkRoomStartX && x <= darkRoomEndX && y >= darkRoomStartY && y <= darkRoomEndY);
}

void Board::print(const std::vector<Player>& players, std::vector<FailMonster>& monsters, const std::vector<Switch>& switches) const {
	if (isGlobalSilent()) return;

	for (int i = 0; i <= BOTTOM_WALL_Y; i++) {
		for (int j = 0; j < WIDTH; j++) {

			char c = map[i][j];
			char charToPrint = c;
			Color colorToUse = Color::WHITE;

			bool hasMonster = false;
			for (const auto& m : monsters) {
				if (m.getX() == j && m.getY() == i) {
					hasMonster = true;
					break;
				}
			}

			if (hasMonster) {
				charToPrint = FAIL_MONSTER;
				colorToUse = Color::RED;
			}

			bool inDarkZone = isInsideDarkRoom(j, i);
			bool light = isLit(j, i, players);

			bool isStatic = (c == WALL_H || c == WALL_V || c == CORNER ||
				(c >= DOOR_NUMBER_1 && c <= DOOR_NUMBER_9));

			if (inDarkZone && !light && !isStatic && c != TORCH) {
				charToPrint = DARK_FLOOR;
				colorToUse = Color::DARKGREY;
			}
			else {
				if (c == DARK_FLOOR && light && !hasMonster) charToPrint = SPACE;

				if (charToPrint == SPACE) colorToUse = Color::WHITE;
				else if (c == WALL_H || c == WALL_V || c == CORNER) colorToUse = Color::LIGHTGREY;
				else if (c == KEY) colorToUse = Color::YELLOW;
				else if (c == BOMB) colorToUse = Color::LIGHTRED;
				else if (c == RIDDLE) colorToUse = Color::LIGHTGREEN;
				else if (c >= DOOR_NUMBER_1 && c <= DOOR_NUMBER_9) colorToUse = Color::CYAN;
				else if (c == SWITCH_LATCHING || c == SWITCH_SMART || c == SWITCH_ACTIVE) {
					colorToUse = Color::BROWN;
					for (const auto& s : switches) {
						if (s.getX() == j && s.getY() == i) {
							if (s.isRegular()) {
								colorToUse = (c == SWITCH_ACTIVE) ? Color::GREEN : Color::RED;
							}
							break;
						}
					}
				}
				else if (c == TORCH) colorToUse = Color::LIGHTCYAN;
				else if (hasMonster && charToPrint == FAIL_MONSTER) colorToUse = Color::RED;
				else if (c == SPRING) colorToUse = Color::GREEN;
				else if (c == OBSTACLE) colorToUse = Color::YELLOW;
			}

			bool forceRedraw = (Board::isWalkable(c) || hasMonster) && charToPrint != SPACE;

			if (forceRedraw || charToPrint != lastBuffer[i][j] || (int)colorToUse != lastColors[i][j]) {
				gotoxy(j, i);
				setTextColor(colorToUse);
				std::cout << charToPrint;

				lastBuffer[i][j] = charToPrint;
				lastColors[i][j] = (int)colorToUse;
			}
		}
	}
	setTextColor(Color::WHITE);
}