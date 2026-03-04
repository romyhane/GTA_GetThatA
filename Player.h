#pragma once
#include "Point.h"
#include "Board.h"
#include "GameUI.h"
#include "Constants.h"
#include <vector>
#include <string>
#include <iostream>
#include <windows.h>


class Player {
private:
	Point place;
	char shape;

	char right, down, left, up, stay;
	char disposeElem;

	char myPocket;
	bool active;

	int lives;
	int score;

	int forceX;
	int forceY;

	int remainingSteps;
	bool inSpringMode;

	int pendingDirX;
	int pendingDirY;

	int springDx;
	int springDy;

	int springSpeed;
	int springTimer;

	bool springReleaseRequested = false;

	bool exitedLevel = false;

	void moveSpringStep(Board& board, std::vector<Player>& allPlayers);
	void moveUnderForce(Board& board, const std::vector<Player>& players);

	void handleDoor(int nextX, int nextY, Board& board, GameUI& ui);
	void collectItem(char itemType, int x, int y, Board& board, GameUI& ui, const std::string& msg);

public:
	Player(char r, char d, char l, char u, char s, char disEl, char sh);

	const Point& getPosition() const { return place; }
	void setPosition(int x, int y);

	void reset();

	bool consumeExitedLevel() { bool r = exitedLevel; exitedLevel = false; return r; }
	void deactivate() { active = false; }
	bool isActive() const { return active; }
	void activate() { active = true; }

	void processKeyPress(char key, Board& board);
	void move(Board& board, GameUI& ui, std::vector<Player>& allPlayers);

	void draw() const { place.draw(shape); }
	void erase() const { place.draw(SPACE); }
	char getPocketItem() const { return myPocket; }

	bool hasTorch() const { return myPocket == TORCH; }

	int getX() const { return place.getX(); }
	int getY() const { return place.getY(); }
	char getShape() const { return shape; }
	void setDir(int x, int y) { place.setDir(x, y); }

	int getLives() const { return lives; }
	int getScore() const { return score; }
	void addScore(int points) { score += points; }
	void loseLife() { if (lives > 0) lives--; }
	void resetStats() { lives = INITIAL_LIVES; score = INITIAL_SCORE; }

	int getForce() const { return isUnderForce() ? springSpeed : PLAYER_STRENGTH; }
	int getDirX() const { if (isUnderForce()) return springDx; return pendingDirX != 0 ? pendingDirX : (place.getNextX() - place.getX()); }
	int getDirY() const { if (isUnderForce()) return springDy; return pendingDirY != 0 ? pendingDirY : (place.getNextY() - place.getY()); }

	void applyForce(int dirX, int dirY, int steps);
	void startSpringFlight(int compressedChars, int dirX, int dirY);

	bool isUnderForce() const { return springTimer > 0; }
	void setSpringMode(bool mode){inSpringMode = mode;pendingDirX = 0;pendingDirY = 0;springReleaseRequested = false;}

	bool isSpringReleaseRequested() const { return springReleaseRequested; }
	void clearPendingLaunch() { pendingDirX = 0; pendingDirY = 0;}


	bool isInSpringMode() const { return inSpringMode; }
	bool hasPendingLaunch() const { return pendingDirX != 0 || pendingDirY != 0; }
	void getPendingLaunch(int& dx, int& dy) const { dx = pendingDirX; dy = pendingDirY; }
	void setPocketContent(char item) { myPocket = item; }
	void requestSpringRelease() { springReleaseRequested = true; }
	bool consumeSpringReleaseRequest() {bool r = springReleaseRequested;springReleaseRequested = false;return r;}

};