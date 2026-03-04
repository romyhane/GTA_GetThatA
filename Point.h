//we took some inspiration from the code we did with Amir on wednesday, tirgul2,
//but did some crucial changes to fit our needs, such as: 
//we didnt need the enum for the screen dimensions (this is handled by the Board class)
//we changed the move function: we have boarders so we didnt want our player to appear 
//at the other side of the screen if we reached the wall!
//also added new functions as shown below :
#pragma once
#include <iostream>
#include "utils.h"

class Point {
private:
	int x, y;
	int dir_x, dir_y;//where the point is heading to
public:
	Point();
	Point(int x, int y);

	void set(int x, int y);
	void setDir(int dir_x, int dir_y);

	int getX() const { return x; }
	int getY() const { return y; }

	int getNextX() const { return x + dir_x; }
	int getNextY() const { return y + dir_y; }

	void draw(char ch) const;
	void move(char ch);
};