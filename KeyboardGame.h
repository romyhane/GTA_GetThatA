#pragma once
#include "Game.h"
#include <fstream> 

class KeyboardGame : public Game {
private:
	bool isSaveMode;
	std::ofstream stepsFile;
	std::ofstream resultFile;
public:
	KeyboardGame(bool save);
	virtual ~KeyboardGame();
	virtual char getNextInput() override;
	virtual void reportEvent(const std::string& eventName, const std::string& details) override;
};