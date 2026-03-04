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

class Spring {
private:
	std::vector<Point> bodyParts;//holds the coordinates of each segment of the spring
	std::vector<Point> recoveryQueue;//queue used to rebuild the spring after compression
	int recoveryTimer;//controls speed or recovery
	int recoveryIndex;//current index being recovered
	bool isRecovering;//flag to indicate if the spring is currently growing back
	bool waitingForClearToRebuild = false;
	int collapsedUntilIndex = -1; // 0..len-1, where 0 is anchor (wall side)
	bool isAnyPlayerOnSpring(const std::vector<Player>& players) const;



public:
	Spring() : recoveryTimer(0), recoveryIndex(0), isRecovering(false) {}

	void init(int startX, int startY, int length, bool isVertical = false);
	bool contains(int x, int y) const;//checks if a specific coordinate (x, y) belongs to this spring
	int getLength() const { return (int)bodyParts.size(); }

	//returns the base position of the spring
	Point getAnchor() const { if (!bodyParts.empty()) return bodyParts[0]; return Point(0, 0); }

	int getPartIndex(int x, int y) const;
	Point getPart(int index) const { if (index >= 0 && index < bodyParts.size()) return bodyParts[index];  return Point(0, 0); }

	//starts the logic to regenerate the spring after it has been used
	void triggerRecovery(int launchDx, int launchDy, Board& board);
	void update(Board& board, const std::vector<Player>& players);

	char getShape() const { return SPRING; }
	void checkCollision(Player& p, Board& board, GameUI& ui, std::vector<Player>& allPlayers);
};
