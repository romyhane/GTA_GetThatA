#include "Spring.h"
#include "Player.h"
#include <iostream>
#include "utils.h"
#include <algorithm>

void Spring::init(int startX, int startY, int length, bool isVertical) {
	bodyParts.clear();
	isRecovering = false;

	//create body parts
	for (int i = 0; i < length; i++) {
		if (isVertical) { bodyParts.emplace_back(startX, startY + i); }
		else { bodyParts.emplace_back(startX + i, startY); }
	}
    auto isWallAdjacent = [](const Point& p) {
        return (p.getX() == LEFT_WALL_X + 1) ||
            (p.getX() == RIGHT_WALL_X - 1) ||
            (p.getY() == TOP_WALL_Y + 1) ||
            (p.getY() == BOTTOM_WALL_Y - 1);
        };

    if (!bodyParts.empty()) {
        bool frontAdj = isWallAdjacent(bodyParts.front());
        bool backAdj = isWallAdjacent(bodyParts.back());

        //if only the back is wall-adjacent, reverse so front becomes the anchor.
        if (!frontAdj && backAdj) {
            std::reverse(bodyParts.begin(), bodyParts.end());
        }
    }
}

bool Spring::contains(int x, int y) const {
	if (isRecovering) return false;

	for (const auto& p : bodyParts) { if (p.getX() == x && p.getY() == y) return true; }
	return false;
}

int Spring::getPartIndex(int x, int y) const {
	if (isRecovering) return NOT_FOUND;
	for (size_t i = 0; i < bodyParts.size(); i++) {
		if (bodyParts[i].getX() == x && bodyParts[i].getY() == y) {
			return (int)i;
		}
	}
	return NOT_FOUND;
}

void Spring::triggerRecovery(int launchDx, int launchDy, Board& board) {
	isRecovering = true;
	recoveryTimer = 0;
	recoveryIndex = 0;
	recoveryQueue = bodyParts;

	for (const auto& p : bodyParts) { board.addObject(p.getX(), p.getY(), SPACE); }

	//sort the recovery queue so the spring grows from the anchor outwards.
	//we use the launch direction to determine which parts are "closer" to the base.
	std::sort(recoveryQueue.begin(), recoveryQueue.end(),
		[launchDx, launchDy](const Point& a, const Point& b) {
			int scoreA = a.getX() * launchDx + a.getY() * launchDy;
			int scoreB = b.getX() * launchDx + b.getY() * launchDy;
			return scoreA < scoreB;
		});
}

void Spring::update(Board& board, const std::vector<Player>& allPlayers) {
    if (waitingForClearToRebuild)
    {
        if (!isAnyPlayerOnSpring(allPlayers))
        {
            //rebuild in one game cycle (no animation, no sleep)
            for (int i = 0; i < getLength(); i++)
            {
                Point part = getPart(i);
                board.addObject(part.getX(), part.getY(), SPRING);
            }

            waitingForClearToRebuild = false;
            collapsedUntilIndex = -1;
        }
        else
        {
            //keep the spring collapsed while occupied (prevents partial redraw glitches)
            for (int i = 0; i < getLength(); i++)
            {
                Point part = getPart(i);
                if (i <= collapsedUntilIndex) board.addObject(part.getX(), part.getY(), SPRING);
                else                          board.addObject(part.getX(), part.getY(), SPACE);
            }
        }
        return;
    }
	if (!isRecovering) return;

	recoveryTimer++;

	//control the speed of the animation using the constant
	if (recoveryTimer >= SPRING_RECOVERY_DELAY) {
		recoveryTimer = 0;

		if (recoveryIndex < recoveryQueue.size()) {//draw next part of spring
			Point p = recoveryQueue[recoveryIndex];
			setTextColor(Color::LIGHTGREEN);
			board.addObject(p.getX(), p.getY(), SPRING);
			setTextColor(Color::WHITE);
			recoveryIndex++;
		}
		else {//finished recovering
			isRecovering = false;
		}
	}
}

void Spring::checkCollision(Player& p, Board& board, GameUI& ui, std::vector<Player>& allPlayers)
{
    (void)allPlayers;

    if (p.isUnderForce()) return; // if already launched, ignore.

    int idx = getPartIndex(p.getX(), p.getY());
    if (idx == NOT_FOUND) {
        return;
    }

    Point anchor = getPart(0);
    Point tip = getPart(getLength() - 1);

    //release direction: from wall-side to free-side.
    int releaseDx = 0, releaseDy = 0;
    if (tip.getX() > anchor.getX()) releaseDx = DIR_POSITIVE;
    else if (tip.getX() < anchor.getX()) releaseDx = DIR_NEGATIVE;

    if (tip.getY() > anchor.getY()) releaseDy = DIR_POSITIVE;
    else if (tip.getY() < anchor.getY()) releaseDy = DIR_NEGATIVE;

    //compression direction is toward the wall (toward anchor), i.e. opposite release direction.
    int compressDx = -releaseDx;
    int compressDy = -releaseDy;

    if (!p.isInSpringMode()) {
        p.setSpringMode(true);
        p.setDir(compressDx, compressDy);
    }

    // VISUAL COLLAPSE:
    for (int i = 0; i < getLength(); i++) {
        Point part = getPart(i);
        if (i <= idx) board.addObject(part.getX(), part.getY(), SPRING);
        else          board.addObject(part.getX(), part.getY(), SPACE);
    }

    // RELEASE TRIGGERS:
    // 1. reached the wall-side end (idx == 0)
    bool reachedWall = (idx == 0);

    // 2. STAY key requested release
    bool stayRelease = p.consumeSpringReleaseRequest();

    // 3. attempted to change direction (anything other than continuing compression)
    bool dirChangeRelease = false;
    int pdx = 0, pdy = 0;

    if (p.hasPendingLaunch()) {
        p.getPendingLaunch(pdx, pdy);

        // if player asked to keep moving toward the wall, that's NOT a direction change.
        if (pdx == compressDx && pdy == compressDy) {
            p.clearPendingLaunch();
        }
        else {
            dirChangeRelease = true;
        }
    }

    if (!(reachedWall || stayRelease || dirChangeRelease)) {
        return; // still charging
    }

    int compressedChars = getLength() - idx;
    if (compressedChars < 1) compressedChars = 1;

    //restore spring to full length
    waitingForClearToRebuild = true;
    collapsedUntilIndex = idx;

    if (dirChangeRelease) {
        if ((pdx == releaseDx && pdy == releaseDy) || (pdx == -releaseDx && pdy == -releaseDy)) {
            p.setDir(0, 0);
        }
        else {
            p.setDir(pdx, pdy); // lateral direction
        }
        p.clearPendingLaunch();
    }
    else {
        p.setDir(0, 0);
    }

    p.startSpringFlight(compressedChars, releaseDx, releaseDy);

    if (!isGlobalSilent()) {
        ui.printMessage("BOING! Speed: " + std::to_string(compressedChars));
    }
}

bool Spring::isAnyPlayerOnSpring(const std::vector<Player>& players) const
{
    for (const auto& p : players)
    {
        if (!p.isActive()) continue;
        if (getPartIndex(p.getX(), p.getY()) != NOT_FOUND)
            return true;
    }
    return false;
}
