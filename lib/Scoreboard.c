//
// Created by emad on 10/02/2022.
//

#include <string.h>
#include "Perimeters.h"

#include "Scoreboard.h"

// Sorting the current standing
void SORT_SCOREBOARD(struct Scoreboard *sb){
    fflush(stdout);
    for(int i=sb->userCnt;i>0;i--){
        for(int j=1;j<i;j++){
            if((sb->scoreList[j])>(sb->scoreList[j-1])){
                char tmpn[50];
                strcpy(tmpn, sb->nameList[j]);
                strcpy(sb->nameList[j], sb->nameList[j-1]);
                strcpy(sb->nameList[j-1], tmpn);
                int tmp=sb->scoreList[j];
                sb->scoreList[j]=sb->scoreList[j-1];
                sb->scoreList[j-1]=tmp;
            }
        }
    }
}

// Adding a new user to standing
void NEW_TO_SCOREBOARD(struct Scoreboard *sb, char *name, int score){
    strcpy(sb->nameList[sb->userCnt], name);
    sb->scoreList[sb->userCnt]=score;
    sb->userCnt++;
    SORT_SCOREBOARD(sb);
}

// Adding some points to a user in standing
void ADD_TO_SCOREBOARD(struct Scoreboard *sb, char *name, int val){
    for(int i=0;i<sb->userCnt;i++){
        if(strcmp(name, sb->nameList[i])==0){
            sb->scoreList[i]+=val;
            SORT_SCOREBOARD(sb);
            return ;
        }
    }
    NEW_TO_SCOREBOARD(sb, name, val);
}