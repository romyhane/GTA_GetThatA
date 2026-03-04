#include "GameUI.h"
#include <iostream>
#include <conio.h>
#include "utils.h"
#include "Constants.h"
#include "Player.h"
#include <cstdlib>
#include <windows.h>

using std::cout;
using std::endl;
using std::string;

void GameUI::printDualColorLine(const std::string& leftText, Color leftColor,
	const std::string& rightText, Color rightColor) {
	if (isGlobalSilent()) return;
	setTextColor(leftColor);
	cout << leftText;
	setTextColor(rightColor);
	cout << rightText << endl;
	setTextColor(Color::WHITE);
}

void GameUI::clearScreen() {
	if (isGlobalSilent()) return;
	system("cls");// clears the console screen for windows
}

char GameUI::showMainMenu() {
	if (isGlobalSilent()) return MENU_OPTION_EXIT;
	while (true) {
		clearScreen();

		cout << "\n\n";
		setTextColor(Color::LIGHTCYAN);
		// ofc we used ascii art generator for this:
		// we choose a cool font from patorjk.com
		cout << "      /$$$$$$  /$$$$$$$$ /$$$$$$  " << endl;
		cout << "     /$$__  $$|__  $$__//$$__  $$ " << endl;
		cout << "    | $$  \\__/   | $$  | $$  \\ $$ " << endl;
		cout << "    | $$ /$$$$   | $$  | $$$$$$$$ " << endl;
		cout << "    | $$|_  $$   | $$  | $$__  $$ " << endl;
		cout << "    | $$  \\ $$   | $$  | $$  | $$ " << endl;
		cout << "    |  $$$$$$/   | $$  | $$  | $$ " << endl;
		cout << "     \\______/    |__/  |__/  |__/ " << endl;
		setTextColor(Color::WHITE);
		cout << "\n";
		setTextColor(Color::YELLOW);
		cout << "      ~ G E T   T H A T   A ~    " << endl;
		setTextColor(Color::WHITE);
		cout << "  ===============================" << endl;
		setTextColor(Color::YELLOW);
		cout << "   Romy & Tomer vs. The Semester " << endl;
		setTextColor(Color::WHITE);
		cout << "\n";
		setTextColor(Color::LIGHTGREY);
		cout << "    [1] START GAME " << endl;
		cout << "    [5] TOGGLE COLORS (B&W \\ ";
		setTextColor(Color::RED);
		cout << "C";
		setTextColor(Color::BROWN);
		cout << "O";
		setTextColor(Color::GREEN);
		cout << "L";
		setTextColor(Color::LIGHTBLUE);
		cout << "O";
		setTextColor(Color::MAGENTA);
		cout << "R";
		setTextColor(Color::LIGHTGREY);
		cout << ")" << endl;
		cout << "    [7] SCORING SYSTEM " << endl;
		cout << "    [8] INSTRUCTIONS & KEYS " << endl;
		cout << "    [9] EXIT " << endl;
		setTextColor(Color::WHITE);
		char key = _getch();
		if (key == TOGGLE_COLOR_KEY) {
			toggleColorMode();
		}
		else {
			return key;
		}
	}
}

void GameUI::showInstructions() {
	if (isGlobalSilent()) return;

	while (true) {
		clearScreen();
		cout << "\n";
		setTextColor(Color::LIGHTCYAN);
		cout << "          =========================" << endl;
		cout << "              MEET YOUR PLAYERS    " << endl;
		cout << "          =========================" << endl;
		setTextColor(Color::WHITE);
		// for the drawings we asked GEMINI for some help 
		printDualColorLine("        @_____@           ", Color::LIGHTMAGENTA, "        _______       ", Color::LIGHTBLUE);
		printDualColorLine("       |  ^ ^  |          ", Color::LIGHTMAGENTA, "       |  o o  |      ", Color::LIGHTBLUE);
		printDualColorLine("       |   v   |          ", Color::LIGHTMAGENTA, "       |   -   |      ", Color::LIGHTBLUE);
		printDualColorLine("        \\_____/           ", Color::LIGHTMAGENTA, "        \\_____/       ", Color::LIGHTBLUE);
		printDualColorLine("         /   \\            ", Color::LIGHTMAGENTA, "         _|_|_        ", Color::LIGHTBLUE);
		printDualColorLine("       _/  R  \\_          ", Color::LIGHTMAGENTA, "        /  T  \\       ", Color::LIGHTBLUE);
		printDualColorLine("      (_________)         ", Color::LIGHTMAGENTA, "       /_______\\      ", Color::LIGHTBLUE);
		printDualColorLine("        ROMY (R)          ", Color::LIGHTMAGENTA, "       TOMER (T)     ", Color::LIGHTBLUE);

		cout << "\n";
		cout << "   Controls (Left):           Controls (Right):" << endl;

		cout << "   Move: "; setTextColor(Color::YELLOW); cout << "W, A, X, D"; setTextColor(Color::WHITE);
		cout << "           Move: "; setTextColor(Color::YELLOW); cout << "I, J, M, L" << endl; setTextColor(Color::WHITE);

		cout << "   Stop: "; setTextColor(Color::YELLOW); cout << "S"; setTextColor(Color::WHITE);
		cout << "                    Stop: "; setTextColor(Color::YELLOW); cout << "K" << endl; setTextColor(Color::WHITE);

		cout << "   Throw: "; setTextColor(Color::YELLOW); cout << "E"; setTextColor(Color::WHITE);
		cout << "                   Throw: "; setTextColor(Color::YELLOW); cout << "O" << endl; setTextColor(Color::WHITE);

		cout << "\n";
		setTextColor(Color::LIGHTCYAN); cout << " INSTRUCTIONS :" << endl; setTextColor(Color::WHITE);

		cout << " 1. Use the "; setTextColor(Color::YELLOW); cout << "KEYS (K)"; setTextColor(Color::WHITE);
		cout << " to open "; setTextColor(Color::CYAN); cout << "DOORS (1-8)." << endl; setTextColor(Color::WHITE);
		cout << " 2. Watch out for "; setTextColor(Color::LIGHTRED); cout << "BOMBS (@)", setTextColor(Color::WHITE); cout << " and", setTextColor(Color::LIGHTRED);
		cout << " MONSTERS (F)"; setTextColor(Color::WHITE); cout << " !" << endl;

		cout << " 3. Pick up "; setTextColor(Color::LIGHTCYAN); cout << "TORCH (!)"; setTextColor(Color::WHITE);
		cout << " to see in the dark." << endl;

		cout << " 4. Step on "; setTextColor(Color::BROWN); cout << "SWITCHES (\\)"; setTextColor(Color::WHITE);
		cout << " to control doors." << endl;

		cout << " 5. Solve "; setTextColor(Color::LIGHTGREEN); cout << "RIDDLES (?)"; setTextColor(Color::WHITE);
		cout << " & use "; setTextColor(Color::LIGHTGREEN); cout << "SPRING (#)"; setTextColor(Color::WHITE);
		cout << " to genrate force and push "; setTextColor(Color::YELLOW); cout << "OBSTACLES(*)." << endl;
		setTextColor(Color::WHITE);
		cout << "\n Press [5] to toggle colors or any key to return menu...";

		char key = _getch();
		if (key == TOGGLE_COLOR_KEY) {
			toggleColorMode();
		}
		else {
			return;
		}
	}
}

void GameUI::showVictoryScreen(int totalScore) {
	if (isGlobalSilent()) return;

	while (true) {
		clearScreen();
		cout << "\n\n\n";
		setTextColor(Color::YELLOW);
		cout << "       * * * * * * * * * * * * * * * * * " << endl;
		cout << "       *                               * " << endl;
		cout << "       *     MISSION ACCOMPLISHED!     * " << endl;
		cout << "       *       YOU GOT THAT 'A'!       * " << endl;
		cout << "       *        CONGRATULATIONS        * " << endl;
		cout << "       *                               * " << endl;
		if (totalScore <= SCORE_ALIGNMENT_THRESHOLD) {
			cout << "       *       TOTAL SCORE: " << totalScore << "        * " << endl;
		}
		else {
			cout << "       *       TOTAL SCORE: " << totalScore << "       * " << endl;
		}
		cout << "       *                               * " << endl;
		cout << "       * * * * * * * * * * * * * * * * * " << endl;
		setTextColor(Color::WHITE);
		cout << "\n\n";
		cout << " Press [5] to toggle colors or any key to finish...";
		char key = _getch();
		if (key == TOGGLE_COLOR_KEY) {
			toggleColorMode();
		}
		else {
			return;
		}
	}
}

void GameUI::showGameOver() {
	if (isGlobalSilent()) return;

	while (true) {
		clearScreen();
		cout << "\n\n";
		setTextColor(Color::RED);
		cout << "       :( :( :( :( :( :( :( :( :(      " << endl;
		cout << "                                       " << endl;
		cout << "               GAME OVER               " << endl;
		cout << "          Try again next time...       " << endl;
		cout << "                                       " << endl;
		cout << "       :( :( :( :( :( :( :( :( :(      " << endl;
		setTextColor(Color::WHITE);
		cout << "\n\n";
		cout << " Press [5] to toggle colors or any key to return menu...";
		char key = _getch();
		if (key == TOGGLE_COLOR_KEY) {
			toggleColorMode();
		}
		else {
			return;
		}
	}
}

void GameUI::printMessage(const std::string& msg, bool force) {
	if (!force && msg == lastMessage) {
		return;
	}
	lastMessage = msg;

	if (isGlobalSilent()) {
		return;
	}

	gotoxy(MSG_LINE_X, MSG_LINE_Y);
	//clear previous message
	cout << "                                                                                                                        ";

	//print new message
	gotoxy(MSG_LINE_X, MSG_LINE_Y);
	setTextColor(Color::YELLOW);
	cout << "MSG: " << msg;
	setTextColor(Color::WHITE);
}

void GameUI::printWallCollisionMsg(char playerShape) {
	static const char* msgs[] = {
		"SegFault! Just kidding.",
		"Wall is 'const'. Can't move.",
		"Maybe try the door next time?",
		"Nope. Can't ghost through that.",
		"Blocked like my brain in an exam.",
		"Even ChatGPT can't help here.",
		"Do you need glasses? Or a map?",
		"That hit harder than Algebra 2.",
		"Move! I need my 100.",
		"Blocking my Dean's List spot.",
		"Bonk! That hurt.",
		"Ouchie. Wall.",
		"Just checking if it's solid... Yep.",
		"Calculus was easier than this."
	};

	int index = rand() % NUM_FUNNY_MSGS;

	std::string msg = std::string(1, playerShape) + ": " + msgs[index];
	printMessage(msg);
}

void GameUI::printPlayerStatus(int x, int y, const std::string& name, const Player& p, Color color, bool useGraphics) {
	if (isGlobalSilent()) return;

	gotoxy(x, y);
	setTextColor(color);
	cout << name << ": ";
	cout.flush();

	setTextColor(Color::LIGHTRED);
	int lives = p.getLives();
	if (lives < 0) lives = 0;

	if (useGraphics) {//use Windows API to print the Unicode Heart Symbol
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		DWORD written;
		for (int i = 0; i < lives; i++) {
			WriteConsoleW(hConsole, HEART_SYMBOL, 2, &written, NULL);
		}
	}
	else {//print L
		for (int i = 0; i < lives; i++) {
			std::cout << LIFE_SYMBOL_TEXT;
		}
	}

	for (int i = lives; i < MAX_LIVES; i++) { cout << "  "; } //print spaces for lost lives

	// print a message indicating the player's status
	gotoxy(x, y + 1);
	setTextColor(color);
	cout << "Score: " << p.getScore() << " | Inventory: ";

	char item = p.getPocketItem();
	if (item != 0) {
		cout << "[ ";
		if (item == KEY) setTextColor(Color::YELLOW);
		else if (item == BOMB) setTextColor(Color::LIGHTRED);
		else if (item == TORCH) setTextColor(Color::LIGHTCYAN);
		else setTextColor(Color::WHITE);

		cout << item;
		setTextColor(color);
		cout << " ]    ";
	}
	else {
		cout << "[ Empty ]    ";
	}
}

void GameUI::printLegend(int startX, int startY, const std::vector<Player>& players, bool useGraphics) {
	if (isGlobalSilent()) return;

	printPlayerStatus(startX, startY, "ROMY", players[ROMY_INDEX], Color::LIGHTMAGENTA, useGraphics);
	printPlayerStatus(startX + SEP_ROMY_TOMER_LEGEND, startY, "TOMER", players[TOMER_INDEX], Color::LIGHTBLUE, useGraphics);
	setTextColor(Color::WHITE);
}

void GameUI::showScoringSystem() {
	if (isGlobalSilent()) return;
	while (true) {
		clearScreen();
		setTextColor(Color::LIGHTCYAN);
		cout << "\n";
		cout << " ========================================================== \n";
		cout << " |               S C O R I N G   S Y S T E M              | \n";
		cout << " ========================================================== \n\n";
		setTextColor(Color::WHITE);
		cout << "             COMPETITIVENESS MEETS COOPERATION! \n\n";

		cout << "\n";
		setTextColor(Color::LIGHTGREEN);
		cout << "                     --- SCORING TABLE --- \n";
		setTextColor(Color::WHITE);


		cout << " Riddle:    100/75/50 pts     |   Level Complete: 100 pts\n";
		cout << " Key:       50 pts            |   Lose Life:      -25 pts\n";
		cout << " Bomb Use:  20 pts            |   Push Obstacle:  5 pts per step\n";

		cout << "\n";
		setTextColor(Color::LIGHTMAGENTA);
		cout << " TIP: Teamwork (Pushing Obstacles) gives the most infinite points!\n";
		setTextColor(Color::WHITE);

		cout << "\n";
		cout << " Press [5] to toggle colors or any other key to return...";

		char key = _getch();


		if (key == TOGGLE_COLOR_KEY) {
			toggleColorMode();
		}
		else {
			return;
		}
	}
}