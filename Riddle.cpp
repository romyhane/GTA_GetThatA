#include "Riddle.h"
#include "Game.h"
#include "utils.h"

Riddle::Riddle() : position(0, 0) {}

void Riddle::init(int x, int y, const std::string& q, const std::string& a) {
    position.set(x, y);
    question = q;
    answer = a;

    isSolved = false;
    attempts = 0;

    engaged = false;
    engagedPlayerIndex = -1;
}

int Riddle::checkAnswerChar(char c) {
    attempts++;

    if (answer.empty()) return 0;

    if (answer.size() != 1) return 0;

    return (toLowerCase(c) == toLowerCase(answer[0])) ? scoreForAttempt(attempts) : 0;
}

bool Riddle::tryActivate(int playerIndex, const Player& p, GameUI& ui, const Game* game) {
    if (isSolved || engaged) return false;
    if (p.getX() != position.getX() || p.getY() != position.getY()) return false;

    engaged = true;
    engagedPlayerIndex = playerIndex;

    if (!game->getIsSilentMode()) {
        ui.printMessage("RIDDLE: " + question + " (STUCK TIL SOLVED)");
    }

    return true;
}

void Riddle::enforceStuck(Player& p) const {
    if (p.getX() != position.getX() || p.getY() != position.getY()) {
        p.setPosition(position.getX(), position.getY());
    }
}

bool Riddle::processKey(char key,
    Player& p,
    Board& board,
    GameUI& ui,
    Game* game,
    std::vector<Player>& players,
    std::vector<FailMonster>& monsters,
    const std::vector<Switch>& switches) {
    if (!engaged || isSolved) return false;
    if (key == 0) return false;

    if (key == ESC) {
        if (game->shouldShowMenu()) {
            if (!game->getIsSilentMode()) {
                ui.printMessage("Left riddle (not solved).", true);
            }
            p.setPosition(p.getX() - 1, p.getY());
            disengage();
        }
        return false;
    }

    int pointsEarned = checkAnswerChar(key);

    if (pointsEarned > 0) {
        p.addScore(pointsEarned);

        board.removeObject(position.getX(), position.getY());

        game->reportEvent("RIDDLE_SOLVED", "Q: " + question + " | A: " + answer);

        isSolved = true;
        disengage();

        if (!game->getIsSilentMode()) {
            ui.printMessage("CORRECT! Path is clear.", true);
            board.forceRefresh();
            board.print(players, monsters, switches);

            for (auto& pl : players) {
                if (pl.isActive()) {
                    if (pl.getShape() == CHAR_ROMY) setTextColor(Color::LIGHTMAGENTA);
                    else setTextColor(Color::LIGHTBLUE);
                    pl.draw();
                }
            }
            setTextColor(Color::WHITE);
        }

        return true;
    }

    if (!game->getIsSilentMode()) {
        ui.printMessage("WRONG! Try again.", true);
    }

    return false;
}

void Riddle::disengage() {
    engaged = false;
    engagedPlayerIndex = -1;
}

int Riddle::scoreForAttempt(int attempts) {
    if (attempts == RIDDLE_1ST_ATTEMPT) return RIDDLE_1ST_TRY_SCORE;
    if (attempts == RIDDLE_2ND_ATTEMPT) return RIDDLE_2ND_TRY_SCORE;
    if (attempts == RIDDLE_3RD_ATTEMPT) return RIDDLE_3RD_TRY_SCORE;
    return RIDDLE_MIN_SCORE;
}