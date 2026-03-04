#include "Switch.h"
#include <iostream>
#include "utils.h"

using std::abs;
using std::cout;


Switch::Switch() : position(0, 0), doorPosition(0, 0), doorChar(SPACE), isOn(false), isLatching(false) {}

void Switch::init(int x, int y, bool latching) {
	position.set(x, y);
	doorPosition.set(0, 0);
	doorChar = SPACE;
	isOn = false;
	isLatching = latching;
}

void Switch::connectToDoor(Point doorPos, char doorCh) {
	this->doorPosition = doorPos;
	this->doorChar = doorCh;
}

char Switch::getShape() const {
	if (isLatching) {
		return isOn ? SWITCH_ACTIVE : SWITCH_LATCHING;
	}
	return SWITCH_SMART;//always ^
}

void Switch::checkPressure(Board& board, const std::vector<Player>& players, GameUI& ui) {
	bool isPlayerOnSwitch = false;
	for (const auto& player : players) {
		if (player.isActive() && player.getX() == position.getX() && player.getY() == position.getY()) {
			isPlayerOnSwitch = true;
			break;
		}
	}
	if (doorChar == SPACE) return;

	if (isOn != isPlayerOnSwitch) {

		//REGULAR SWITCH (LATCHING)
		if (isLatching) {
			if (!isOn && isPlayerOnSwitch) {
				isOn = true;

				setTextColor(Color::GREEN);
				board.addObject(position.getX(), position.getY(), getShape());
				setTextColor(Color::WHITE);

				board.removeObject(doorPosition.getX(), doorPosition.getY());
				ui.printMessage("Switch activated! Door opened permanently.");

				for (const auto& player : players) {
					if (player.isActive() && player.getX() == position.getX() && player.getY() == position.getY()) {
						if (player.getShape() == CHAR_ROMY) setTextColor(Color::LIGHTMAGENTA);
						else setTextColor(Color::LIGHTBLUE);
						player.draw();
						break;
					}
				}
				setTextColor(Color::WHITE);
			}
			return;
		}


		//SMART SWITCH
		isOn = isPlayerOnSwitch;
		int dx = doorPosition.getX();
		int dy = doorPosition.getY();
		char targetContent = board.getCharAt(dx, dy);

		//update door state
		if (isOn) {//open door

			if (targetContent == doorChar) { //door is closed
				while (dy > TOP_WALL_Y && board.getCharAt(dx, dy - 1) == doorChar) {
					dy--;
				}

				doorPosition.set(dx, dy);

				board.removeObject(dx, dy);
				ui.printMessage("Hold it! Door acts as long as you stand.");
			}
			else if (targetContent == SPACE) {//door already open
				ui.printMessage("Door already open.");
			}
			else {//something else is blocking the door
				ui.printMessage("Error: Door blocked or missing.");
			}
		}

		else {//close door
			bool isPlayerBlocking = false;

			for (const auto& player : players) {
				if (player.isActive()) {
					if (player.getX() == dx && player.getY() >= dy && player.getY() < dy + DOOR_LEN) {
						isPlayerBlocking = true;
						break;
					}
				}
			}
			if (!isPlayerBlocking) {
				setTextColor(Color::CYAN);
				board.addObject(dx, dy, doorChar, DOOR_LEN);
				setTextColor(Color::WHITE);
				ui.printMessage("Switch released! Door closed.");
			}
			else {//player is standing on the doorway
				ui.printMessage("Cannot close! Blocked by player.");
			}
		}
	}
}



void Switch::draw(const std::vector<Player>& players) const {
	for (const auto& player : players) {
		if (player.isActive() && player.getX() == position.getX() && player.getY() == position.getY()) {
			return;
		}
	}
	if (isLatching) {
		// REGULAR SWITCH
		if (isOn) { setTextColor(Color::GREEN); }// ON (/)
		else { setTextColor(Color::RED); }// OFF (\) 
	}
	else {
		// SMART SWITCH (^)
		setTextColor(Color::BROWN);// ALWAYS BROWN
	}
	position.draw(getShape());
	setTextColor(Color::WHITE);
}