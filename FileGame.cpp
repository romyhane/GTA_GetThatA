#include "FileGame.h"
#include <iostream>
#include <cstdlib>

using std::cout;
using std::endl;

FileGame::FileGame(bool silent) : isSilent(silent), nextStepCycle(-1), nextStepKey(0) {
    isSilentMode = silent;
    stepsFile.open("adv-world.steps");
    if (!stepsFile.is_open()) {
        cout << "Error: Could not open 'adv-world.steps' for reading!" << endl;
        return;
    }

    unsigned int seed;
    if (stepsFile >> seed) {
        srand(seed);
    }
    else return;

    if (!(stepsFile >> nextStepCycle >> nextStepKey)) {
        nextStepCycle = -1;
    }
    expectedResultFile.open("adv-world.result");
    if (!expectedResultFile.is_open()) {
        cout << "Error: Could not open 'adv-world.result' for verification!" << endl;
    }
    loadNextExpectedEvent();
}


FileGame::~FileGame() {
    if (stepsFile.is_open()) {
        stepsFile.close();
    }
}

char FileGame::getNextInput() {
    if (!stepsFile.is_open() || nextStepCycle == -1) {
        return 0;
    }

    if (gameCycle == nextStepCycle) {
        char keyToReturn = nextStepKey;

        if (!(stepsFile >> nextStepCycle >> nextStepKey)) {
            nextStepCycle = -1;
        }
        return keyToReturn;
    }
    return 0;
}

void FileGame::updateGraphics() {
    //in silent mode, do not update graphics
    if (isSilentMode) return;

    Game::updateGraphics();
}

void FileGame::loadNextExpectedEvent() {
    if (!expectedResultFile.is_open()) {
        nextExpectedEvent.isValid = false;
        return;
    }

    if (expectedResultFile >> nextExpectedEvent.time >> nextExpectedEvent.name) {
        std::string line;
        std::getline(expectedResultFile, line);

        size_t firstChar = line.find_first_not_of(" \t\r");
        if (firstChar != std::string::npos) {
            nextExpectedEvent.details = line.substr(firstChar);
        }
        else {
            nextExpectedEvent.details = "";
        }

        if (!nextExpectedEvent.details.empty() && nextExpectedEvent.details.back() == '\r') {
            nextExpectedEvent.details.pop_back();
        }

        nextExpectedEvent.isValid = true;
    }
    else {
        nextExpectedEvent.isValid = false;
    }
}

void FileGame::reportEvent(const std::string& eventName, const std::string& details) {
    if (!nextExpectedEvent.isValid) {
        if (!isSilentMode) system("cls");
        std::cout << "TEST FAILED: Event happened but EOF reached in result file." << std::endl;
        std::cout << "Event: " << eventName << " at cycle " << gameCycle << std::endl;
        exit(1);
    }

    bool timeMatch = (nextExpectedEvent.time == gameCycle);
    bool nameMatch = (nextExpectedEvent.name == eventName);
    bool detailsMatch = (nextExpectedEvent.details == details);

    //strict match against the expected results file.
    if (timeMatch && nameMatch && detailsMatch) {
        loadNextExpectedEvent();
        if (isSilentMode && !nextExpectedEvent.isValid) {
            std::cout << "TEST PASSED\n";
            exit(0);
        }
    }
    else {
        if (!isSilentMode) system("cls");

        std::cout << "TEST FAILED at cycle " << gameCycle << "!" << std::endl;
        std::cout << "Expected: " << nextExpectedEvent.time << " " << nextExpectedEvent.name
            << " (" << nextExpectedEvent.details << ")" << std::endl;
        std::cout << "Actual:   " << gameCycle << " " << eventName
            << " (" << details << ")" << std::endl;
        exit(1);
    }
}