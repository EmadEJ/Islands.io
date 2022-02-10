//
// Created by emad on 02/02/2022.
//

#ifndef TEST_GRAPHICS_FILEHANDLING_H
#define TEST_GRAPHICS_FILEHANDLING_H

#include "Scoreboard.h"

void SAVE_MAP(struct Map inp, char *name);

struct Map LOAD_MAP(char *name);

void SAVE_GAME(struct Map inp);

struct Map LOAD_GAME();

void SAVE_SCOREBOARD(struct Scoreboard inp);

struct Scoreboard LOAD_SCOREBOARD();

void FILECOUNT(char *path, int *cnt, char *mapName);

int FILEEXISTS(const char *path);

#endif //TEST_GRAPHICS_FILEHANDLING_H
