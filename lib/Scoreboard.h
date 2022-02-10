//
// Created by emad on 03/02/2022.
//

#ifndef TEST_GRAPHICS_SCOREBOARD_H
#define TEST_GRAPHICS_SCOREBOARD_H

struct Scoreboard{
    int userCnt;
    char nameList[MAX_USER][50];
    int scoreList[MAX_USER];
};

void SORT_SCOREBOARD(struct Scoreboard *sb);

void NEW_TO_SCOREBOARD(struct Scoreboard *sb, char *name, int score);

void ADD_TO_SCOREBOARD(struct Scoreboard *sb, char *name, int val);

#endif //TEST_GRAPHICS_SCOREBOARD_H
