#include <iostream>
#include <cstdlib> 
#include <ctime>   
#include <string>
#include <conio.h>
#include "Game.h"
#include "utils.h" 
#include "KeyboardGame.h"
#include "FileGame.h"

using namespace std;

int main(int argc, char* argv[]) {
	bool isLoad = false;
	bool isSave = false;
	bool isSilent = false;

	for (int i = 1; i < argc; ++i) {
		string arg = argv[i];
		if (arg == "-load") isLoad = true;
		if (arg == "-save") isSave = true;
		if (arg == "-silent") isSilent = true;
	}

	Game* game = nullptr;

	setGlobalSilent(isLoad && isSilent);

	if (isLoad) {
		if (!isSilent) {
			cout << "Starting Game in LOAD mode..." << endl;
		}
		game = new FileGame(isSilent);
	}
	else {
		cout << "Starting Game in KEYBOARD mode..." << endl;
		if (isSave) cout << "(Recording to file...)" << endl;

		game = new KeyboardGame(isSave);
	}

	game->start();

	delete game;
	return 0;
}