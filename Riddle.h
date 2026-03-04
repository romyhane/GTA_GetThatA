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
#include "Switch.h"
#include "FailMonster.h"

class Player;
class Game;

class Riddle {
private:
	Point position;
	std::string question;
	std::string answer;
	bool isSolved = true;
	int attempts = 0;

	bool engaged = false;
	int engagedPlayerIndex = -1;

	int checkAnswerChar(char c);
public:
	Riddle();

	void init(int x, int y, const std::string& q, const std::string& a);


	char getShape() const { return RIDDLE; }
	int getX() const { return position.getX(); }
	int getY() const { return position.getY(); }

	bool getIsSolved() const { return isSolved; }
	bool isEngaged() const { return engaged; }
	int getEngagedPlayerIndex() const { return engagedPlayerIndex; }

	bool processKey(char key,
		Player& p,
		Board& board,
		GameUI& ui,
		Game* game,
		std::vector<Player>& players,
		std::vector<FailMonster>& monsters,
		const std::vector<Switch>& switches);

	int scoreForAttempt(int attempts);
	void disengage();
	void enforceStuck(Player& p) const;
	bool tryActivate(int playerIndex, const Player& p, GameUI& ui, const Game* game);
};