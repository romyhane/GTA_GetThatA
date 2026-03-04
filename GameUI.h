#pragma once
#include <string>
#include <vector>
#include "utils.h"

class Player;

class GameUI {
private:
	// Helper function to print a line with two different colors, 
	// we got some help from GEMINI with this function.
	static void printDualColorLine(const std::string& leftText,
		Color leftColor, const std::string& rightText, Color rightColor);
	std::string lastMessage = "";

	void printPlayerStatus(int x, int y, const std::string& name, const Player& p, Color color, bool useGraphics);
public:
	static void clearScreen();
	static char showMainMenu();
	static void showInstructions();
	static void showVictoryScreen(int totalScore);
	static void showGameOver();
	static void showScoringSystem();

	void printMessage(const std::string& msg, bool force = false);
	void printWallCollisionMsg(char playerShape);
	void printLegend(int startX, int startY, const std::vector<Player>& players, bool useGraphics);
	void restoreMessage() { printMessage(lastMessage, true); }
};