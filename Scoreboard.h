//
// Created by emad on 03/02/2022.
//

#ifndef TEST_GRAPHICS_SCOREBOARD_H
#define TEST_GRAPHICS_SCOREBOARD_H

#include <string.h>
#include "Perimeters.h"

struct Scoreboard{
    int userCnt;
    char *nameList[MAX_USER];
    int scoreList[MAX_USER];
};

void SORT_SCOREBOARD(struct Scoreboard *sb){
    for(int i=sb->userCnt-1;i>0;i--){
        for(int j=1;j<i;j++){
            if(sb->scoreList[j]>sb->scoreList[j-1]){
                char *tmpn=sb->nameList[j];
                sb->nameList[j]=sb->nameList[j-1];
                sb->nameList[j-1]=tmpn;
                int tmp=sb->scoreList[j];
                sb->scoreList[j]=sb->scoreList[j-1];
                sb->scoreList[j-1]=tmp;
            }
        }
    }
}

void NEW_TO_SCOREBOARD(struct Scoreboard *sb, char *name, int score){
    sb->nameList[sb->userCnt]=name;
    sb->scoreList[sb->userCnt]=score;
    sb->userCnt++;
}

void ADD_TO_SCOREBOARD(struct Scoreboard *sb, char *name, int val){
    for(int i=0;i<sb->userCnt;i++){
        if(strcmp(name, sb->nameList[i])==0){
            sb->scoreList[i]+=val;
            return ;
        }
    }
}

#endif //TEST_GRAPHICS_SCOREBOARD_H
