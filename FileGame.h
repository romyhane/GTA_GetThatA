#pragma once
#include "Game.h"
#include <fstream>
#include <string>

class FileGame : public Game {
private:
	bool isSilent;
	std::ifstream stepsFile;
	std::ifstream expectedResultFile;

	long nextStepCycle;
	char nextStepKey;

	struct ExpectedEvent {
		long time;
		std::string name;
		std::string details;
		bool isValid = false;
	} nextExpectedEvent;

	void loadNextExpectedEvent();
public:
	FileGame(bool silent);
	virtual ~FileGame();

	virtual char getNextInput() override;
	virtual void updateGraphics() override;
	virtual bool shouldShowMenu() const override { return false; }

	virtual void reportEvent(const std::string& eventName, const std::string& details) override;
};

