// this cpp file was created with Amir on wednesday, tirgul2 
//we added toLowerCase, setTextColor, isColorEnabled and toggleColorMode
#include <iostream>
#include <windows.h> // Required for Windows API console functions
#include "utils.h"

static bool colorEnabled = true;
static bool globalSilent = false;

void gotoxy(int x, int y) {
	if (globalSilent) return;
	std::cout.flush();
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void hideCursor(){
	if (globalSilent) return;
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO curInfo;
	GetConsoleCursorInfo(hStdOut, &curInfo);
	curInfo.bVisible = FALSE; // Set to TRUE to make it visible
	SetConsoleCursorInfo(hStdOut, &curInfo);
}

char toLowerCase(char c) { return (c >= UPPER_A && c <= UPPER_Z) ? c + CASE_DIFF : c; }

void toggleColorMode() {
	colorEnabled = !colorEnabled;
}

bool isColorEnabled() {
	return colorEnabled;
}

void setTextColor(Color color) {
	if (globalSilent) return;
	if (colorEnabled) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (int)color);
	}
	else {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (int)Color::WHITE);
	}
}

void setGlobalSilent(bool silent) {
	globalSilent = silent;
}

bool isGlobalSilent() {
	return globalSilent;
}
