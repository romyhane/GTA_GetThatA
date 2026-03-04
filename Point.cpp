#include "Point.h"
#include "Constants.h"

Point::Point() {
	x = DEFAULT_X; y = DEFAULT_Y;
	dir_x = 0; dir_y = 0;
}

Point::Point(int x, int y) {
	this->x = x;
	this->y = y;
	dir_x = 0; dir_y = 0;
}

void Point::set(int x, int y) {
	this->x = x;
	this->y = y;
}

void Point::setDir(int dir_x, int dir_y) {
	this->dir_x = dir_x;
	this->dir_y = dir_y;
}

void Point::draw(char ch) const {
	if (isGlobalSilent()) return;

	gotoxy(x, y);
	std::cout << ch;
}

void Point::move(char ch) {
	if (dir_x == 0 && dir_y == 0) return;
	//else, we are moving

	draw(SPACE); //erase me first

	//update position
	x += dir_x;
	y += dir_y;

	//draw at new position
	draw(ch);
}