#pragma once
#include <vector>
#include <string>
#include "Board.h"
#include "Player.h"
#include "GameUI.h"
#include "Bomb.h"
#include "Riddle.h"
#include "Switch.h"
#include "Spring.h"
#include "Constants.h"
#include "FailMonster.h"

class Game {
protected:
	Board board;
	GameUI ui;
	int legendX;
	int legendY;

	long gameCycle = 0;

	std::vector<Player> players;
	std::vector<Bomb> bombs;
	std::vector<Riddle> riddles;
	std::vector<Switch> switches;
	std::vector <Spring> springs;
	std::vector<FailMonster> monsters;
	std::vector<std::string> levelFiles;

	bool isSilentMode = false;

	int currentLevel;
	int globalRiddleIndex = 0;
	bool gameOver;
	bool exitGame;
	void buildVictoryRoom();

	void initLevel();
	void handleInput();
	void updateGameLogic();
	void checkGameStatus();
	void pauseGame();
	bool loadLevelFromFile(const std::string& fileName);
	bool verifySystemFiles();

	void findLevelFiles();

	void scanAndBuildSprings();

	int activeRiddleIndex = -1;
	int activeRiddlePlayerIndex = -1;

	struct riddleData {
		std::string question;
		std::string answer;
	};

	void loadRiddlesFromFile(std::vector<riddleData>& outRiddles);

public:
	Game();
	virtual ~Game() {}
	void start();
	void run();
	void tick() { gameCycle++; }

	bool getIsSilentMode() const { return isSilentMode; }
	virtual char getNextInput() = 0;
	virtual void updateGraphics();
	virtual bool shouldShowMenu() const { return true; }
	virtual void reportEvent(const std::string& eventName, const std::string& details = "") {}
};