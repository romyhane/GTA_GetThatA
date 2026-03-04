#include "Bomb.h"
#include <iostream>
#include "utils.h"
#include "Player.h"
#include "Game.h"

using std::abs;
using std::string;
using std::to_string;
using std::cout;

Bomb::Bomb() : position(0, 0), timer(0), isActive(false) {}

void Bomb::activate(int x, int y) {
	position.set(x, y);
	isActive = true;
	timer = BOMB_TICKS_TO_EXPLODE;
}

bool Bomb::tick(Board& board, std::vector<Player>& players, GameUI& ui, bool& gameOver, Game* game) {
	if (!isActive) return false;

	timer--;

	//check if the bomb should explode
	if (timer <= 0) {
		explode(board);
		isActive = false;

		string msg = "BOOM! ";
		bool someoneWasHit = false;
		bool playerDied = false;

		int bombX = position.getX();
		int bombY = position.getY();

		for (auto& p : players) {
			if (p.isActive()) {
				int dx = abs(p.getX() - bombX);
				int dy = abs(p.getY() - bombY);

				//check if the player is within the explosion range
				if (dx <= EXPLOSION_RANGE && dy <= EXPLOSION_RANGE) {
					p.loseLife();
					someoneWasHit = true;

					string name = (p.getShape() == CHAR_ROMY) ? "Romy" : "Tomer";

					if (game) {
						game->reportEvent("PLAYER_DIED", name);
					}

					if (p.getLives() > 0) {
						msg += "OUCH! " + name + " lost a life! left: " + to_string(p.getLives()) + ". ";
					}
					else {
						p.deactivate();
						ui.printMessage("OH NO! " + name + " died!");
						playerDied = true;
					}
				}
			}
		}

		//redraw active players
		for (auto& p : players) {
			if (p.isActive()) {
				if (p.getShape() == CHAR_ROMY) setTextColor(Color::LIGHTMAGENTA);
				else setTextColor(Color::LIGHTBLUE);
				p.draw();
			}
		}
		setTextColor(Color::WHITE);

		//handle outcome: Game Over or just a hit
		if (playerDied) {
			msg += "GAME OVER :(";
			ui.printMessage(msg);
			// For file verification / silent mode, explicitly log game over.
			if (game) {
				game->reportEvent("GAME_OVER", "Bomb");
			}
			if (!isGlobalSilent()) Sleep(GAME_OVER_DELAY);
			gameOver = true;
		}
		else if (someoneWasHit) {
			ui.printMessage(msg);
			if (!isGlobalSilent()) Sleep(HIT_MESSAGE_DELAY);
		}
		else {
			msg += "bomb exploded.";
			ui.printMessage(msg);
		}

		return true;
	}

	if (timer % BLINKING_INTERVAL == 0) {
		position.draw(SPACE);
	}
	else {
		setTextColor(Color::LIGHTRED);
		position.draw(BOMB);
		setTextColor(Color::WHITE);
	}

	return false;
}

void Bomb::explode(Board& board) {
	int centerX = position.getX();
	int centerY = position.getY();

	//iterate over the explosion area square
	for (int i = -EXPLOSION_RANGE; i <= EXPLOSION_RANGE; i++) {
		for (int j = -EXPLOSION_RANGE; j <= EXPLOSION_RANGE; j++) {

			int targetX = centerX + j;
			int targetY = centerY + i;

			if (targetX <= LEFT_WALL_X || targetX >= RIGHT_WALL_X ||
				targetY <= TOP_WALL_Y || targetY >= BOTTOM_WALL_Y) {
				continue;
			}

			char cell = board.getCharAt(targetX, targetY);
			if (cell >= DOOR_NUMBER_1 && cell <= DOOR_NUMBER_9) {
				continue;
			}

			else {
				board.removeObject(targetX, targetY);
			}
		}
	}
}