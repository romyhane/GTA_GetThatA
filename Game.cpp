#include "Game.h"
#include <iostream>
#include <conio.h>
#include <windows.h>
#include "utils.h"
#include <algorithm>
#include <fstream>
#include <filesystem>

using std::cout;
using std::endl;
using std::string;
using std::abs;
namespace fs = std::filesystem;

Game::Game() : currentLevel(1), gameOver(false), exitGame(false), legendX(0), legendY(0), isSilentMode(false), globalRiddleIndex(0)
{
	srand((unsigned int)time(0));
	hideCursor();
	findLevelFiles();
	//initialize players
	players.emplace_back(Player(ROMY_RIGHT, ROMY_DOWN, ROMY_LEFT, ROMY_UP, ROMY_STAY, ROMY_DE, CHAR_ROMY));//Romy
	players.emplace_back(Player(TOMER_RIGHT, TOMER_DOWN, TOMER_LEFT, TOMER_UP, TOMER_STAY, TOMER_DE, CHAR_TOMER));//Tomer
}

void Game::findLevelFiles() {
	levelFiles.clear();
	for (const auto& entry : fs::directory_iterator(".")) {
		if (entry.is_regular_file()) {
			std::string filename = entry.path().filename().string();
			if (filename.rfind("adv-world_", 0) == 0 && filename.find(".screen") != std::string::npos) {
				levelFiles.push_back(filename);
			}
		}
	}
	std::sort(levelFiles.begin(), levelFiles.end());
}

void Game::start() {
	if (!verifySystemFiles()) return;

	if (!shouldShowMenu()) {
		for (auto& p : players) { p.resetStats(); }
		currentLevel = LEVEL_1;
		run();
		return;
	}

	while (!exitGame) {
		char choice = ui.showMainMenu();

		if (choice == MENU_OPTION_START) {
			for (auto& p : players) { p.resetStats(); }
			currentLevel = LEVEL_1;
			globalRiddleIndex = 0;
			run();
		}
		else if (choice == MENU_OPTION_INSTRUCTIONS) {
			ui.showInstructions();
		}
		else if (choice == MENU_OPTION_SCORING) {
			ui.showScoringSystem();
		}
		else if (choice == MENU_OPTION_EXIT) {
			exitGame = true;
			ui.clearScreen();
			cout << "Exited game. Goodbye!" << endl;
		}
	}
}

void Game::run() {
	bombs.clear();
	riddles.clear();
	switches.clear();
	ui.clearScreen();

	board.setDarkRoom(false);

	if (currentLevel > levelFiles.size() || currentLevel < 1) {
		std::cout << "Error: Invalid level index: " << currentLevel << std::endl;
		return;
	}

	std::string fileName = levelFiles[currentLevel - 1];

	if (!loadLevelFromFile(fileName)) {
		std::cout << "CRITICAL ERROR: Failed to load level: " << fileName << std::endl;
		(void)_getch();
		return;
	}

	scanAndBuildSprings();

	updateGraphics();

	std::string msg = "";
	if (currentLevel == LEVEL_1) {
		msg = "Welcome! Use WAXSD (Romy) & IJMKL (Tomer) to move.";
	}
	else if (currentLevel == LEVEL_2) {
		msg = "Watch out for BOMBS and use Switches properly!";
	}
	else if (currentLevel == LEVEL_3) {
		msg = "The FAIL Monster lurks in the Dark Area with the Key. It's a THRILLER!";
	}
	if (!msg.empty() && !isSilentMode) { ui.printMessage(msg); }

	//main game loop
	gameOver = false;
	gameCycle = 0;

	while (!gameOver) {
		gameCycle++;

		handleInput();
		updateGameLogic();
		checkGameStatus();

		if (gameOver) { break; }

		updateGraphics();
	}
	if (gameOver && !exitGame && shouldShowMenu() && !isSilentMode && currentLevel <= (int)levelFiles.size()) {
		ui.showGameOver();
	}
}

void Game::scanAndBuildSprings() {
	springs.clear();

	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {

			if (board.getCharAt(x, y) == SPRING) {
				bool alreadyHandled = false;
				for (const auto& s : springs) {
					if (s.contains(x, y)) {
						alreadyHandled = true;
						break;
					}
				}
				if (alreadyHandled) continue;
				int lengthX = 0;
				while (x + lengthX < WIDTH && board.getCharAt(x + lengthX, y) == SPRING) {
					lengthX++;
				}

				int lengthY = 0;
				while (y + lengthY < HEIGHT && board.getCharAt(x, y + lengthY) == SPRING) {
					lengthY++;
				}

				Spring newSpring;
				if (lengthX > 1 && lengthX >= lengthY) {
					newSpring.init(x, y, lengthX, false);
				}
				else if (lengthY > 1) {
					newSpring.init(x, y, lengthY, true);
				}
				else {
					newSpring.init(x, y, 1, false);
				}
				springs.push_back(newSpring);
			}
		}
	}
}

void Game::initLevel() {
	// clean from previous level
	board.resetMap();
	board.setDarkRoom(false);
	bombs.clear();
	riddles.clear();
	switches.clear();
	springs.clear();
	monsters.clear();
	ui.clearScreen();


	for (auto& p : players) {
		p.reset();
	}

	if (currentLevel > levelFiles.size()) {

		int totalScore = 0;
		for (const auto& p : players) { totalScore += p.getScore(); }

		reportEvent("GAME_ENDED", std::to_string(totalScore));

		if (shouldShowMenu() && !isSilentMode) {
			buildVictoryRoom();
			ui.showVictoryScreen(totalScore);
		}

		gameOver = true;
		return;
	}

	std::string fileName = levelFiles[currentLevel - 1];

	if (!loadLevelFromFile(fileName)) {
		std::cout << "CRITICAL ERROR: Failed to load level: " << fileName << std::endl;
		(void)_getch();
		gameOver = true;
		exitGame = true;
		return;
	}

	scanAndBuildSprings();
	if (!isSilentMode) {
		board.print(players, monsters, switches);


		setTextColor(Color::LIGHTGREEN);
		for (const auto& s : springs) {
			for (int i = 0; i < s.getLength(); i++) {
				Point p = s.getPart(i);
				gotoxy(p.getX(), p.getY());
				std::cout << s.getShape();
			}
		}
		setTextColor(Color::WHITE);

		setTextColor(Color::LIGHTMAGENTA);
		players[ROMY_INDEX].draw();

		setTextColor(Color::LIGHTBLUE);
		players[TOMER_INDEX].draw();

		setTextColor(Color::WHITE);

		std::string msg = "";
		if (currentLevel == LEVEL_1) {
			msg = "Welcome! Use WASD (Romy) & IJKL (Tomer) to move.";
		}
		else if (currentLevel == LEVEL_2) {
			msg = "Watch out for BOMBS and use Switches properly!";
		}
		else if (currentLevel == LEVEL_3) {//cute reference to michael jackson :)
			msg = "The FAIL Monster lurks in the Dark Area with the Key. It's a THRILLER!";
		}

		if (!msg.empty()) {
			ui.printMessage(msg);
		}
	}
}

void Game::buildVictoryRoom() {
	if (!shouldShowMenu() || isSilentMode) return;

	board.setDarkRoom(false);
	monsters.clear();

	players[ROMY_INDEX].setPosition(MID_X, MID_Y);
	players[TOMER_INDEX].setPosition(MID_X + 2, MID_Y);


	bool inVictoryScreen = true;
	bool needRedraw = true;

	while (inVictoryScreen) {

		if (needRedraw) {
			ui.clearScreen();
			ui.printLegend(legendX, legendY, players, isColorEnabled());
			setTextColor(Color::WHITE);

			for (int x = 1; x < WIDTH; x++) {
				gotoxy(x, TOP_WALL_Y); cout << WALL_H;
				gotoxy(x, BOTTOM_WALL_Y); cout << WALL_H;
			}

			for (int y = TOP_WALL_Y; y <= BOTTOM_WALL_Y; y++) {
				gotoxy(0, y); cout << WALL_V;
				gotoxy(WIDTH - 1, y); cout << WALL_V;
			}

			gotoxy(LEFT_WALL_X, TOP_WALL_Y); cout << CORNER;
			gotoxy(RIGHT_WALL_X, TOP_WALL_Y); cout << CORNER;
			gotoxy(LEFT_WALL_X, BOTTOM_WALL_Y); cout << CORNER;
			gotoxy(RIGHT_WALL_X, BOTTOM_WALL_Y); cout << CORNER;

			for (auto& p : players) {
				if (p.getShape() == CHAR_ROMY) setTextColor(Color::LIGHTMAGENTA);
				else setTextColor(Color::LIGHTBLUE);
				p.draw();
			}
			setTextColor(Color::WHITE);

			ui.printMessage("CONGRATS! You finished the game! Press [5] for colors, or any key to exit.", true);

			needRedraw = false;
		}


		char key = getNextInput();
		if (key == 0) {
			if (!shouldShowMenu()) return;
			continue;
		}

		if (key == TOGGLE_COLOR_KEY) {
			toggleColorMode();
			needRedraw = true;
		}
		else {
			inVictoryScreen = false;
		}
	}

	ui.clearScreen();
}

void Game::handleInput() {
	char key = getNextInput();

	while (key != 0) {

		if (key == ESC) {
			if (shouldShowMenu() && activeRiddleIndex < 0) {
				pauseGame();
			}
			key = getNextInput();
			continue;
		}

		if (key == TOGGLE_COLOR_KEY) {
			toggleColorMode();

			if (!isSilentMode) {
				ui.clearScreen();
				board.forceRefresh();
				board.print(players, monsters, switches);
				ui.restoreMessage();

				for (auto& s : switches) { s.draw(players); }

				// redraw players and bombs
				setTextColor(Color::LIGHTMAGENTA); players[ROMY_INDEX].draw();
				setTextColor(Color::LIGHTBLUE); players[TOMER_INDEX].draw();
				setTextColor(Color::WHITE);

				for (auto& b : bombs) {
					gotoxy(b.getX(), b.getY());
					cout << b.getShape();
				}
			}

			return;
		}

		//riddle handling
		if (activeRiddleIndex >= 0 && activeRiddleIndex < (int)riddles.size()
			&& activeRiddlePlayerIndex >= 0 && activeRiddlePlayerIndex < (int)players.size()) {

			bool solved = riddles[activeRiddleIndex].processKey(
				key,
				players[activeRiddlePlayerIndex],
				board, ui, this,
				players, monsters, switches
			);

			if (solved) {
				activeRiddleIndex = -1;
				activeRiddlePlayerIndex = -1;
			}

			key = getNextInput();
			continue;
		}

		for (auto& p : players) {
			bool hadBomb = (p.getPocketItem() == BOMB);

			p.processKeyPress(key, board);

			if (hadBomb && p.getPocketItem() == EMPTY_POCKET) {
				Bomb newBomb;
				newBomb.activate(p.getX(), p.getY());
				bombs.push_back(newBomb);
				p.addScore(POINTS_FOR_BOMB_USE);
				ui.printMessage("Bomb has been planted. RUN!!!!!!!");
			}
		}

		key = getNextInput();
	}
}

void Game::updateGameLogic() {
	if (activeRiddleIndex >= 0 && activeRiddleIndex < (int)riddles.size()
		&& activeRiddlePlayerIndex >= 0 && activeRiddlePlayerIndex < (int)players.size()) {

		riddles[activeRiddleIndex].enforceStuck(players[activeRiddlePlayerIndex]);
		return;
	}

	// player movement and item collection
	for (auto& p : players) {
		if (p.isActive()) {
			p.move(board, ui, players);

			if (p.consumeExitedLevel()) {
				std::string name = (p.getShape() == CHAR_ROMY) ? "Romy" : "Tomer";
				int fromScreen = currentLevel;
				int toScreen = currentLevel + 1;
				reportEvent("PLAYER_MOVED_SCREEN", name + " from " + std::to_string(fromScreen) + " to " + std::to_string(toScreen));
			}

			int px = p.getX();
			int py = p.getY();
			char item = board.getCharAt(px, py);

			if (item == KEY || item == BOMB || item == TORCH) {
				if (p.getPocketItem() == EMPTY_POCKET) {
					p.setPocketContent(item);
					board.removeObject(px, py);

					std::string name = (item == KEY) ? "Key" : (item == BOMB) ? "Bomb" : "Torch";
					if (!isSilentMode) {
						ui.printMessage("Got a " + name + "!");
					}

					if (item == KEY) {
						p.addScore(POINTS_FOR_KEY);
					}
				}
			}
			else if (item >= DOOR_NUMBER_1 && item <= DOOR_NUMBER_9) {//door handling
				p.setPocketContent(EMPTY_POCKET);
				board.removeObject(px, py);
				if (!isSilentMode) {
					ui.printMessage("Door Opened!");
				}
			}
		}
	}

	for (int ri = 0; ri < (int)riddles.size(); ++ri) {
		if (riddles[ri].getIsSolved() || riddles[ri].isEngaged()) continue;

		for (int pi = 0; pi < (int)players.size(); ++pi) {
			if (!players[pi].isActive()) continue;

			if (riddles[ri].tryActivate(pi, players[pi], ui, this)) {
				activeRiddleIndex = ri;
				activeRiddlePlayerIndex = pi;

				riddles[ri].enforceStuck(players[pi]);
				return;
			}
		}
	}

	for (auto& f : monsters) {
		f.move(board);

		// monster collision check with players
		for (auto& p : players) {
			if (p.isActive() && p.getX() == f.getX() && p.getY() == f.getY()) {
				if (!isSilentMode) {
					ui.printMessage("FAIL! THE FAIL MONSTER CAUGHT YOU. GAME OVER!");
				}
				std::string name = (p.getShape() == CHAR_ROMY) ? "Romy" : "Tomer";
				reportEvent("PLAYER_DIED", name + " (Monster)");
				reportEvent("GAME_OVER", name + " (Monster)");
				gameOver = true;
				return;
			}
		}
	}

	// spring update and collision check
	for (auto& s : springs) {
		s.update(board, players);
		for (auto& p : players) {
			if (p.isActive()) {
				s.checkCollision(p, board, ui, players);
			}
		}
	}

	// spring handling
	for (auto& s : switches) { s.checkPressure(board, players, ui); }
	if (!isSilentMode) {
		for (auto& s : switches) { s.draw(players); }
	}

	// bomb handling
	for (size_t i = 0; i < bombs.size();) {
		bool exploded = bombs[i].tick(board, players, ui, gameOver, this);

		if (exploded) {
			bombs.erase(bombs.begin() + i);

			ui.printLegend(legendX, legendY, players, isColorEnabled());

			if (gameOver) return;
		}
		else {
			i++;
		}
	}
}

void Game::checkGameStatus() {
	bool anyAlive = false;

	for (auto& p : players) {
		if (p.isActive()) {
			anyAlive = true;
			break;
		}
	}

	if (!anyAlive && !gameOver) {
		std::string bonusMsg = "LEVEL COMPLETE! Bonuses: ";
		for (auto& p : players) {
			int livesLost = INITIAL_LIVES - p.getLives();
			if (livesLost < 0) livesLost = 0;

			int bonus = LEVEL_COMPLETE_BONUS - (livesLost * LIFE_PENALTY);
			if (bonus < 0) bonus = 0;

			p.addScore(bonus);

			std::string name = (p.getShape() == CHAR_ROMY) ? "Romy" : "Tomer";
			bonusMsg += name + ": +" + std::to_string(bonus) + " ";
		}

		if (!isSilentMode) {
			ui.clearScreen();
			ui.printMessage(bonusMsg);
			Sleep(TWO_SECONDS_MS);
		}

		
		currentLevel++;
		initLevel();
		return;
	}

}

void Game::pauseGame() {
	ui.printMessage("GAME PAUSED. Press ESC to continue or 'H' for Home.");

	while (true) {
		if (_kbhit()) {
			char key = _getch();

			key = toLowerCase(key);

			if (key == ESC) {
				ui.printMessage("Resuming...");
				return;
			}

			if (key == HOME) {
				gameOver = true;
				return;
			}
		}
	}
}

bool Game::loadLevelFromFile(const std::string& fileName) {
	std::ifstream file(fileName);
	if (!file.is_open()) {
		std::cout << "Error: Could not open level file: " << fileName << std::endl;
		return false;
	}

	std::vector<riddleData> levelRiddles;
	loadRiddlesFromFile(levelRiddles);
	

	struct DoorInfo { int x; int y; char id; };
	std::vector<DoorInfo> tempDoors;

	board.resetMap();
	board.setDarkRoom(false);
	bombs.clear();
	riddles.clear();
	switches.clear();
	springs.clear();
	monsters.clear();

	int minDarkX = WIDTH, maxDarkX = 0;
	int minDarkY = HEIGHT, maxDarkY = 0;
	bool foundDarkness = false;

	std::string line;
	int y = 0;
	bool foundLegend = false;

	while (std::getline(file, line) && y < HEIGHT) {
		for (int x = 0; x < (int)line.length() && x < WIDTH; x++) {
			char ch = line[x];

			if (ch == SWITCH_LATCHING || ch == SWITCH_SMART) {
				Switch s(x, y);
				bool isRegular = (ch == SWITCH_LATCHING);
				s.init(x, y, isRegular);
				switches.push_back(s);
				board.addObject(x, y, ch);
			}

			else if (ch >= DOOR_NUMBER_1 && ch <= DOOR_NUMBER_9) {
				tempDoors.push_back({ x, y, ch });
				board.addObject(x, y, ch);
			}

			else if (ch == CHAR_ROMY) {
				players[ROMY_INDEX].setPosition(x, y);
				board.addObject(x, y, SPACE);
			}

			else if (ch == CHAR_TOMER) {
				players[TOMER_INDEX].setPosition(x, y);
				board.addObject(x, y, SPACE);
			}

			else if (ch == LEGEND) {
				foundLegend = true;
				legendX = x;
				legendY = y;
				board.addObject(x, y, SPACE);
			}

			else if (ch == BOMB) {
				board.addObject(x, y, BOMB);
			}

			else if (ch == TORCH) {
				board.addObject(x, y, TORCH);
			}

			else if (ch == DARK_FLOOR) {
				foundDarkness = true;
				if (x < minDarkX) minDarkX = x;
				if (x > maxDarkX) maxDarkX = x;
				if (y < minDarkY) minDarkY = y;
				if (y > maxDarkY) maxDarkY = y;
				board.addObject(x, y, DARK_FLOOR);
			}

			else if (ch == FAIL_MONSTER) {
				monsters.emplace_back(x, y);
				board.addObject(x, y, SPACE);
			}

			else if (ch == SPRING) {
				board.addObject(x, y, SPRING);
			}

			else if (ch == RIDDLE) {
				Riddle r;
				if (!levelRiddles.empty()) {
					const auto& data = levelRiddles[globalRiddleIndex % levelRiddles.size()];
					r.init(x, y, data.question, data.answer);
					globalRiddleIndex++;
				}
				riddles.push_back(r);
				board.addObject(x, y, ch);
			}

			else if (ch != SPACE) {
				board.addObject(x, y, ch);
			}
		}
		y++;
	}
	file.close();

	//NOTE! we decided to connect each switch to the closest door so switches 
	//wont be hardcoded with door IDs in the map files

	//connect switches to closest doors
	for (auto& s : switches) {
		double minDistance = 100000.0;
		DoorInfo closestDoor = { 0, 0, 0 };
		bool found = false;

		for (const auto& d : tempDoors) {
			//calc distance with Pythagorean theorem
			double dist = std::sqrt(std::pow(s.getX() - d.x, 2) + std::pow(s.getY() - d.y, 2));
			if (dist < minDistance) {
				minDistance = dist;
				closestDoor = d;
				found = true;
			}
		}

		if (found) {
			//connect switch to door
			s.connectToDoor(Point(closestDoor.x, closestDoor.y), closestDoor.id);
		}
	}

	if (foundDarkness) { board.setDarkRoom(true, minDarkX, maxDarkX, minDarkY, maxDarkY); }

	if (y < MIN_LINES || !foundLegend) {
		std::cout << "Error: Map file '" << fileName << "' is incomplete (too few lines)." << std::endl;
		return false;
	}

	return true;
}

bool Game::verifySystemFiles() {
	bool allGood = true;

	if (levelFiles.empty()) {
		std::cout << "CRITICAL ERROR: No level files (adv-world_*.screen) found!" << std::endl;
		allGood = false;
	}

	std::vector<std::string> filesToCheck = { "riddles.txt" };

	filesToCheck.insert(filesToCheck.end(), levelFiles.begin(), levelFiles.end());

	for (const auto& filename : filesToCheck) {
		std::ifstream f(filename);
		if (!f.is_open()) {
			std::cout << "ERROR: Missing file: " << filename << std::endl;
			allGood = false;
		}
		else if (f.peek() == std::ifstream::traits_type::eof()) {
			std::cout << "ERROR: File is empty or corrupted: " << filename << std::endl;
			f.close();
			allGood = false;
		}
		else {
			f.close();
		}
	}

	if (!allGood) {
		std::cout << "\nGame cant start due to missing or corrupt files." << std::endl;
		std::cout << "Please ensure riddles.txt and at least one .screen file exist." << std::endl;
		std::cout << "Press any key to exit";
		(void)_getch();
		return false;
	}

	return true;
}

void Game::loadRiddlesFromFile (std::vector<riddleData>& outRiddles) {
	outRiddles.clear();
	std::ifstream file("riddles.txt");

	if (!file.is_open()) {
		ui.clearScreen();
		std::cout << "Error: Could not open 'riddles.txt'." << std::endl;
		(void)_getch();
		return;
	}

	std::string line;

	while (std::getline(file, line)) {
		if (!line.empty() && line.back() == '\r') line.pop_back();
		if (line.empty()) continue;
		
		bool isHeader = true;
		for (char c : line) {
			if (!isdigit(c)) { isHeader = false; break; }
		}

		if (isHeader) continue;

		riddleData data;
		data.question = line;

		if (std::getline(file, line)) {
			if (!line.empty() && line.back() == '\r') line.pop_back();
			data.answer = line;
			outRiddles.push_back(data);
		}
	}
	file.close();
}

void Game::updateGraphics() {
	board.print(players, monsters, switches);

	for (int i = 0; i < players.size(); i++) {
		if (players[i].isActive()) {
			if (i == ROMY_INDEX) setTextColor(Color::LIGHTMAGENTA);
			else if (i == TOMER_INDEX) setTextColor(Color::LIGHTBLUE);
			players[i].draw();
		}
	}
	setTextColor(Color::WHITE);

	ui.printLegend(legendX, legendY, players, isColorEnabled());
	Sleep(SLEEP_DURATION_MS);
}