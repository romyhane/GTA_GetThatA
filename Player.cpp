#include "Player.h"
#include <iostream>
#include <cmath>
#include <windows.h>

static inline void restoreUnderlyingTile(Board& board, int x, int y){
	// Whatever the board says belongs there (#, space, switch, etc.)
	// write it back using addObject so Board::map and Board::lastBuffer stay consistent.
	const char base = board.getCharAt(x, y);
	board.addObject(x, y, base);
}


Player::Player(char r, char d, char l, char u, char s, char disEl, char sh)
	: right(toLowerCase(r)),
	down(toLowerCase(d)),
	left(toLowerCase(l)),
	up(toLowerCase(u)),
	stay(toLowerCase(s)),
	disposeElem(toLowerCase(disEl)),
	shape(sh),
	myPocket(EMPTY_POCKET),
	place(DEFAULT_X, DEFAULT_Y),
	lives(INITIAL_LIVES),
	score(INITIAL_SCORE),
	active(true),
	forceX(0),
	forceY(0),
	remainingSteps(0),
	inSpringMode(false),
	pendingDirX(0),
	pendingDirY(0),
	springDx(0),
	springDy(0),
	springSpeed(0),
	springTimer(0)
{}


void Player::setPosition(int x, int y) {
	place.set(x, y);
	place.setDir(0, 0);
}

void Player::processKeyPress(char key, Board& board) {
	if (!active) return;
	key = toLowerCase(key);

	//priority 1: handle spring force (flight)
	//if the player is mid-air (spring), controls are disabled or limited
	if (isUnderForce()) {
		int reqDx = 0, reqDy = 0;
		if (key == right) reqDx = DIR_POSITIVE;
		else if (key == left) reqDx = DIR_NEGATIVE;
		else if (key == down) reqDy = DIR_POSITIVE;
		else if (key == up) reqDy = DIR_NEGATIVE;

		if (reqDx == 0 && reqDy == 0) return;

		//prevent stopping or reversing mid-air
		if (reqDx == -springDx && reqDy == -springDy) return;
		if (reqDx == springDx && reqDy == springDy) return;

		place.setDir(reqDx, reqDy);
		return;
	}

	//priority 2: prepare for spring launch
	if (inSpringMode) {
		if (key == stay) {
			requestSpringRelease();
			pendingDirX = 0;
			pendingDirY = 0;
			return;
		}

		// direction change request (also triggers release in the spring logic)
		pendingDirX = 0; pendingDirY = 0;
		if (key == right) pendingDirX = DIR_POSITIVE;
		else if (key == left) pendingDirX = DIR_NEGATIVE;
		else if (key == down) pendingDirY = DIR_POSITIVE;
		else if (key == up) pendingDirY = DIR_NEGATIVE;

		return;
	}

	//priority 3:item disposal (throwing items)
	if (key == disposeElem) {
		// is there an item in my pocket ?
		if (myPocket != EMPTY_POCKET) {
			if (myPocket != BOMB) {board.addObject(place.getX(), place.getY(), myPocket);}

			myPocket = EMPTY_POCKET;
			draw();// redraw player
		}
	}
	//priority 4: standard movement
	else if (key == right) { place.setDir(DIR_POSITIVE, 0); }
	else if (key == down) { place.setDir(0, DIR_POSITIVE); }
	else if (key == left) { place.setDir(DIR_NEGATIVE, 0); }
	else if (key == up) { place.setDir(0, DIR_NEGATIVE); }
	else if (key == stay) { place.setDir(0, 0); }
}

void Player::handleDoor(int nextX, int nextY, Board& board, GameUI& ui) {
	//do we have a key ?
	if (myPocket == KEY) {
		myPocket = EMPTY_POCKET;
		board.removeObject(nextX, nextY); // remove door
		ui.printMessage("Door opened! GO!");

		bool isBoundary = (nextX == LEFT_WALL_X || nextX == RIGHT_WALL_X ||
			nextY == TOP_WALL_Y || nextY == BOTTOM_WALL_Y);

		//if door on boarder => exit == move to next level
		if (isBoundary) {
			place.move(shape);
			erase(); //erase player from old position
			deactivate(); //player exited the level
			return;
		}
		else { //door in borders => same level
			place.move(shape);
		}
	}
	else {//we cant open door
		place.setDir(0, 0); //stuck
		ui.printMessage("Locked! Find a key or activate a Switch.");
	}
}

void Player::collectItem(char itemType, int x, int y, Board& board, GameUI& ui, const std::string& msg) {
	if (myPocket == EMPTY_POCKET) {
		myPocket = itemType;
		board.removeObject(x, y);
		ui.printMessage(msg);
		if (itemType == KEY) {
			addScore(POINTS_FOR_KEY);
		}
	}
}

// THE MAIN MOVEMENT LOGIC
void Player::move(Board& board, GameUI& ui, std::vector<Player>& allPlayers) {
	if (inSpringMode) {
		if (isSpringReleaseRequested() || hasPendingLaunch()) {
			return;
		}
	}

	//spring mode check: if charging a spring, don't move yet
	if (isUnderForce()) {
		for (int i = 0; i < springSpeed; i++) {
			if (springTimer > 0) { moveSpringStep(board, allPlayers); }
			else { break; }
		}

		springTimer--;//decrease flight duration

		//end of flight
		if (springTimer <= 0) {
			springSpeed = 0; springDx = 0; springDy = 0;
			if (shape == CHAR_ROMY) setTextColor(Color::LIGHTMAGENTA);
			else setTextColor(Color::LIGHTBLUE);
			draw();
			setTextColor(Color::WHITE);
		}
	}

	//calculate desired next position
	int nextX = place.getNextX();
	int nextY = place.getNextY();

	if (nextX <= 0) return;//safety check

	//romy and tomer got stuck
	for (const auto& otherPlayer : allPlayers) {
		if (&otherPlayer != this && otherPlayer.isActive() &&
			otherPlayer.getX() == nextX && otherPlayer.getY() == nextY) {

			place.setDir(0, 0);
			ui.printMessage("Whoa! Personal space, please! Players got stuck.");
			return;
		}
	}

	char nextCell = board.getCharAt(nextX, nextY);

	//obstacle pushing logic (Co-op mechanic)
	if (nextCell == OBSTACLE) {
		int dx = nextX - getX();
		int dy = nextY - getY();

		int totalForce = getForce();

		std::vector<Player*> contributors;
		contributors.push_back(this);

		//check if partner is helping
		for (auto& otherPlayer : allPlayers) {
			//partner must be moving in the same direction
			if (&otherPlayer != this && otherPlayer.isActive()) {
				if (otherPlayer.getDirX() == dx && otherPlayer.getDirY() == dy) {

					//partner must be adjacent to the obstacle cluster
					int partnerNextX = otherPlayer.getX() + dx;
					int partnerNextY = otherPlayer.getY() + dy;
					if (board.getCharAt(partnerNextX, partnerNextY) == OBSTACLE) {
						totalForce += otherPlayer.getForce();
						contributors.push_back(&otherPlayer);
					}
				}
			}
		}

		//attempt to push the obstacle cluster
		if (board.tryPushObstacle(nextX, nextY, dx, dy, totalForce, allPlayers)) {
			ui.printMessage("That was heavy, but we are moving it!");
			if (shape == CHAR_ROMY) setTextColor(Color::LIGHTMAGENTA);
			else setTextColor(Color::LIGHTBLUE);

			for (auto* p : contributors) {
				p->addScore(POINTS_FOR_OBSTACLE);
			}

			place.move(shape);
			setTextColor(Color::WHITE);
			return;
		}
		else {
			ui.printMessage("It won't budge! Need more force.");
			return;
		}
	}

	//level boundary exit check
	bool isBoundary = (nextX == LEFT_WALL_X || nextX == RIGHT_WALL_X ||
		nextY == TOP_WALL_Y || nextY == BOTTOM_WALL_Y);

	if (isBoundary && nextCell == SPACE) {
		erase();
		exitedLevel = true;
		deactivate();
		return;
	}

	// door interaction
	if (nextCell >= DOOR_NUMBER_1 && nextCell <= DOOR_NUMBER_9) {
		if (myPocket == KEY) {
			if (shape == CHAR_ROMY) setTextColor(Color::LIGHTMAGENTA);
			else setTextColor(Color::LIGHTBLUE);

			place.move(shape);

			setTextColor(Color::WHITE);
		}
		else {
			place.setDir(0, 0);
			ui.printMessage("Locked! Find a key or activate a Switch.");
		}
		return;
	}

	//standard walkable tiles (space, items...)
	if (Board::isWalkable(nextCell)) {
		if (shape == CHAR_ROMY) { setTextColor(Color::LIGHTMAGENTA); }
		else if (shape == CHAR_TOMER) { setTextColor(Color::LIGHTBLUE); }

		place.move(shape);
		setTextColor(Color::WHITE);

		if (nextCell == SWITCH_LATCHING || nextCell == SWITCH_SMART || nextCell == SWITCH_ACTIVE) { place.setDir(0, 0); }

	}
	else {
		place.setDir(0, 0);
		ui.printWallCollisionMsg(shape);
	}
}

void Player::applyForce(int dirX, int dirY, int steps) {
	forceX = dirX;
	forceY = dirY;
	remainingSteps = steps;
	inSpringMode = false;
}

//handles forced movement (e.g., sliding or being pushed by physics)
void Player::moveUnderForce(Board& board, const std::vector<Player>& players) {
	if (shape == CHAR_ROMY) { setTextColor(Color::LIGHTMAGENTA);}
	else if (shape == CHAR_TOMER) { setTextColor(Color::LIGHTBLUE);}

	while (remainingSteps > 0) {
		int nextX = place.getX() + forceX;
		int nextY = place.getY() + forceY;
		char cell = board.getCharAt(nextX, nextY);

		if (!Board::isWalkable(cell)) {
			remainingSteps = 0;
			break;
		}

		// check collision with other players during slide
		bool hitPlayer = false;
		for (const auto& p : players) {
			if (&p != this && p.isActive() && p.getX() == nextX && p.getY() == nextY) {
				hitPlayer = true;
				break;
			}
		}

		if (hitPlayer) {
			remainingSteps = 0;
			break;
		}

		//update board + position. Rendering must be fully suppressed in -load -silent.
		restoreUnderlyingTile(board, place.getX(), place.getY());
		place.set(nextX, nextY);
		draw();

		if (!isGlobalSilent()) Sleep(SLIDE_DELAY_MS);
		remainingSteps--;
	}

	draw();
}

void Player::startSpringFlight(int compressedChars, int dirX, int dirY) {
	springSpeed = compressedChars;
	springTimer = compressedChars * compressedChars;//duration is proportional to power squared
	springDx = dirX;
	springDy = dirY;
	inSpringMode = false;
}

void Player::moveSpringStep(Board& board, std::vector<Player>& allPlayers) {
	int nextX = place.getX() + springDx;
	int nextY = place.getY() + springDy;
	char cell = board.getCharAt(nextX, nextY);

	if (Board::isWalkable(cell)) {
		//chain reaction: hitting another player mid-air transfers the momentum!
		for (auto& p : allPlayers) {
			if (&p != this && p.isActive() && p.getX() == nextX && p.getY() == nextY) {
				p.startSpringFlight(springSpeed, springDx, springDy);
				p.springTimer = this->springTimer;

				this->springTimer = 0;
				this->springSpeed = 0;
				return;
			}
		}

		//proceed with normal spring move
		restoreUnderlyingTile(board, place.getX(), place.getY());
		place.set(nextX, nextY);

		if (shape == CHAR_ROMY) setTextColor(Color::LIGHTMAGENTA);
		else setTextColor(Color::LIGHTBLUE);
		draw();

		return;
	}

	if (cell == OBSTACLE) {
		int totalForce = getForce();
		std::vector<Player*> contributors;
		contributors.push_back(this);

		//check for help from partner
		for (auto& otherPlayer : allPlayers) {
			if (&otherPlayer != this && otherPlayer.isActive()) {
				if (otherPlayer.getDirX() == springDx && otherPlayer.getDirY() == springDy) {
					int pNextX = otherPlayer.getX() + springDx;
					int pNextY = otherPlayer.getY() + springDy;
					if (board.getCharAt(pNextX, pNextY) == OBSTACLE) {
						totalForce += otherPlayer.getForce();
						contributors.push_back(&otherPlayer);
					}
				}
			}
		}

		//try push
		if (board.tryPushObstacle(nextX, nextY, springDx, springDy, totalForce, allPlayers)) {
			restoreUnderlyingTile(board, place.getX(), place.getY());
			place.set(nextX, nextY);

			if (shape == CHAR_ROMY) setTextColor(Color::LIGHTMAGENTA);
			else setTextColor(Color::LIGHTBLUE);
			draw();


			for (auto* p : contributors) {
				p->addScore(POINTS_FOR_OBSTACLE);
			}
		}
		else { springTimer = 0; }//crash if push failed
		return;
	}
	springTimer = 0;
}

void Player::reset() {
	activate();
	myPocket = EMPTY_POCKET;
	place.setDir(0, 0);
	exitedLevel = false;
}