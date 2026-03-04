#include "KeyboardGame.h"
#include <conio.h>
#include <iostream>
#include <ctime>
#include <cstdlib>

KeyboardGame::KeyboardGame(bool save) : isSaveMode(save) {
	if (isSaveMode) {
		stepsFile.open("adv-world.steps");
		resultFile.open("adv-world.result");

		unsigned int seed = (unsigned int)time(0);

		srand(seed);

		if (stepsFile.is_open()) {
			stepsFile << seed << std::endl;
		}
	}
	else {
		srand((unsigned int)time(0));
	}
}

KeyboardGame::~KeyboardGame() {
	if (stepsFile.is_open()) stepsFile.close();
	if (resultFile.is_open()) resultFile.close();
}

char KeyboardGame::getNextInput() {
	if (_kbhit()) {
		char ch = _getch();

		if (isSaveMode && stepsFile.is_open()) {
			stepsFile << gameCycle << " " << ch << std::endl;
		}
		return ch;
	}
	return 0;
}

void KeyboardGame::reportEvent(const std::string& eventName, const std::string& details) {
	if (isSaveMode && resultFile.is_open()) {
		resultFile << gameCycle << " " << eventName << " " << details << std::endl;
	}
}