// this header file was created with Amir on wednesday, tirgul2
#pragma once
# include "Constants.h"

enum class	Color {
	BLACK = 0,
	BLUE = 1,
	GREEN = 2,
	CYAN = 3,
	RED = 4,
	MAGENTA = 5,
	BROWN = 6,
	LIGHTGREY = 7,
	DARKGREY = 8,
	LIGHTBLUE = 9,
	LIGHTGREEN = 10,
	LIGHTCYAN = 11,
	LIGHTRED = 12,
	LIGHTMAGENTA = 13,
	YELLOW = 14,
	WHITE = 15
};

void gotoxy(int x, int y);
void hideCursor();
char toLowerCase(char c);
void setTextColor(Color color);
bool isColorEnabled();
void toggleColorMode();

// global silent-mode flag (used to fully suppress any screen rendering in -load -silent).
// in silent mode we still allow minimal PASS/FAIL output from the test harness.
void setGlobalSilent(bool silent);
bool isGlobalSilent();