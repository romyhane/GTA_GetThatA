#pragma once

//default values for (x,y) coordinates
constexpr int DEFAULT_X = 1;
constexpr int DEFAULT_Y = 1;

//screen constants
constexpr int WIDTH = 80;
constexpr int HEIGHT = 25;

//actual wall positions
constexpr int RIGHT_WALL_X = 79;
constexpr int BOTTOM_WALL_Y = 23;
constexpr int LEFT_WALL_X = 0;
constexpr int TOP_WALL_Y = 2;

//middle of screen 
constexpr int MID_X = WIDTH / 2 - 1;//40
constexpr int MID_Y = HEIGHT / 2 - 1; // 11

//door characters are '1' to '9'
constexpr char DOOR_NUMBER_1 = '1';
constexpr char DOOR_NUMBER_2 = '2';
constexpr char DOOR_NUMBER_3 = '3';
constexpr char DOOR_NUMBER_4 = '4';
constexpr char DOOR_NUMBER_5 = '5';
constexpr char DOOR_NUMBER_6 = '6';
constexpr char DOOR_NUMBER_7 = '7';
constexpr char DOOR_NUMBER_8 = '8';
constexpr char DOOR_NUMBER_9 = '9';
constexpr int DOOR_LEN = 4;

//wall visuals
constexpr char WALL_V = '|';//vertical wall
constexpr char WALL_H = '-';//horizontal wall
constexpr char CORNER = '+';//corners

//good for many things :)
constexpr char SPACE = ' ';

//collectibles & objects
constexpr char KEY = 'K';
constexpr char BOMB = '@';
constexpr char RIDDLE = '?';
constexpr char SWITCH_LATCHING = '\\';
constexpr char SWITCH_ACTIVE = '/';
constexpr char SWITCH_SMART = '^';
constexpr char TORCH = '!';
constexpr char DARK_FLOOR = '~';
constexpr char OBSTACLE = '*';
constexpr char SPRING = '#';
constexpr char FAIL_MONSTER = 'F';
constexpr int LIGHT_RADIUS = 5;
constexpr char WALKABLE_CHARS[] = " K#@?/\\!~^";

//players
constexpr char CHAR_ROMY = 'R';
constexpr char CHAR_TOMER = 'T';
constexpr int PLAYER_STRENGTH = 1;
constexpr int ROMY_INDEX = 0;
constexpr int TOMER_INDEX = 1;

// Romy's controls
constexpr char ROMY_UP = 'w';
constexpr char ROMY_DOWN = 'x';
constexpr char ROMY_LEFT = 'a';
constexpr char ROMY_RIGHT = 'd';
constexpr char ROMY_STAY = 's';
constexpr char ROMY_DE = 'e';

// Tomer's controls
constexpr char TOMER_UP = 'i';
constexpr char TOMER_DOWN = 'm';
constexpr char TOMER_LEFT = 'j';
constexpr char TOMER_RIGHT = 'l';
constexpr char TOMER_STAY = 'k';
constexpr char TOMER_DE = 'o';

// bomb settings 
constexpr int EXPLOSION_RANGE = 3;
//enough game cycles to see the bomb tick til explosion:
constexpr int BOMB_TICKS_TO_EXPLODE = 10;
constexpr int HIT_MESSAGE_DELAY = 2000;
constexpr int BLINKING_INTERVAL = 2;
constexpr int GAME_OVER_DELAY = 3000;

//pocket
constexpr int EMPTY_POCKET = 0;

//message:
constexpr int MSG_LINE_X = 0;
constexpr int MSG_LINE_Y = 24;
constexpr int NUM_FUNNY_MSGS = 14;
constexpr int LEGEND_MAX_HEIGHT = 3;
constexpr int LEGEND_MAX_WIDTH = 20;

//for toLowerCase function
constexpr char UPPER_A = 'A';
constexpr char UPPER_Z = 'Z';
constexpr int CASE_DIFF = 32;

//quit game
constexpr int ESC = 27;

//main menu options
constexpr char MENU_OPTION_START = '1';
constexpr char TOGGLE_COLOR_KEY = '5';
constexpr char MENU_OPTION_SCORING = '7';
constexpr char MENU_OPTION_INSTRUCTIONS = '8';
constexpr char MENU_OPTION_EXIT = '9';
constexpr char HOME = 'h';
constexpr int TWO_SECONDS_MS = 2000;
constexpr int SLEEP_DURATION_MS = 85;

//levels
constexpr int LEVEL_1 = 1;
constexpr int LEVEL_2 = 2;
constexpr int LEVEL_3 = 3;
constexpr int LAST_BUILT_LEVEL = LEVEL_3;
constexpr int VICTORY_STATE_FLAG = 99;

//legend
constexpr char LEGEND = 'L';
constexpr int INITIAL_LIVES = 3;
constexpr int MAX_LIVES = 3;	
constexpr int MAX_LEGEND_LINES = 2;
constexpr int INITIAL_SCORE = 0;
constexpr int SEP_ROMY_TOMER_LEGEND = 35;
constexpr int MIN_LINES = 20;

//scoring system
constexpr int POINTS_FOR_KEY = 50;
constexpr int POINTS_FOR_BOMB_USE = 20;
constexpr int LEVEL_COMPLETE_BONUS = 100;
constexpr int LIFE_PENALTY = 25;
constexpr int POINTS_FOR_OBSTACLE = 5;
constexpr int SCORE_ALIGNMENT_THRESHOLD = 999;

//riddle
constexpr int RIDDLE_1ST_ATTEMPT = 1;
constexpr int RIDDLE_2ND_ATTEMPT = 2;
constexpr int RIDDLE_3RD_ATTEMPT = 3;
constexpr int RIDDLE_1ST_TRY_SCORE = 100;
constexpr int RIDDLE_2ND_TRY_SCORE = 75;
constexpr int RIDDLE_3RD_TRY_SCORE = 50;
constexpr int RIDDLE_MIN_SCORE = 25;

//spring
constexpr int NOT_FOUND = -1;
constexpr int SPRING_RECOVERY_DELAY = 2;
constexpr int SPRING_COMPRESSION_DELAY = 100;
constexpr int DIR_POSITIVE = 1;
constexpr int DIR_NEGATIVE = -1;

//animation delays
constexpr int SLIDE_DELAY_MS = 50;

// UI & visual elements
constexpr const wchar_t* HEART_SYMBOL = L"\u2665 "; //unicode Heart
constexpr const char* LIFE_SYMBOL_TEXT = "L ";      //fallback for non-graphics mode

// Board & Algorithms
constexpr int NUM_DIRECTIONS = 4; //used for BFS (up,down,left,right)
constexpr int BRESENHAM_MULTIPLIER = 2; //part of the line-of-sight calculation formula
