//
// Created by emad on 02/02/2022.
//

#ifndef TEST_GRAPHICS_FILEHANDLING_H
#define TEST_GRAPHICS_FILEHANDLING_H

#include "Scoreboard.h"

// Saving and loading the maps
void SAVE_MAP(struct Map inp, char *name);

struct Map LOAD_MAP(char *name);

// Saving and loading the last game
void SAVE_GAME(struct Map inp);

struct Map LOAD_GAME();

// Saving and loading the ScoreBoard
void SAVE_SCOREBOARD(struct Scoreboard inp);

struct Scoreboard LOAD_SCOREBOARD();

// Counting files in a directory
int FILECOUNT(char *path);

int FILEEXISTS(const char *path);

#endif //TEST_GRAPHICS_FILEHANDLING_H
