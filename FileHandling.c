//
// Created by emad on 10/02/2022.
//

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "Map.h"
#include "Scoreboard.h"

#include "FileHandling.h"

// Saving and loading the maps
void SAVE_MAP(struct Map inp, char *name){
    char path[50];
    memset(path, 0, 50);
    strcpy(path, "../Maps/");
    strcat(path, name);
    strcat(path, ".dat");
    FILE *fp;
    fp= fopen(path, "w");
    fwrite(&inp, sizeof(struct Map), 1, fp);
    fclose(fp);
}

// loading previously saved map
struct Map LOAD_MAP(char *name){
    char path[50];
    memset(path, 0, 50);
    strcpy(path, "../Maps/");
    strcat(path, name);
    FILE *fp;
    fp= fopen(path, "r");
    struct Map res;
    fread(&res, sizeof(struct Map), 1, fp);
    fclose(fp);
    return res;
}

// Saving the last game
void SAVE_GAME(struct Map inp){
    const char *path= "../lastGame.dat";
    FILE *fp;
    fp= fopen(path, "w");
    fwrite(&inp, sizeof(struct Map), 1, fp);
    fclose(fp);
}

// loading the last saved game
struct Map LOAD_GAME(){
    const char *path= "../lastGame.dat";
    FILE *fp;
    fp= fopen(path, "r");
    struct Map res;
    fread(&res, sizeof(struct Map), 1, fp);
    fclose(fp);
    return res;
}

// Saving the ScoreBoard
void SAVE_SCOREBOARD(struct Scoreboard inp){
    const char *path= "../scoreboard.dat";
    FILE *fp;
    fp= fopen(path, "w");
    fwrite(&inp, sizeof(struct Scoreboard), 1, fp);
    fclose(fp);
}

// Loading scoreboard
struct Scoreboard LOAD_SCOREBOARD(){
    const char *path= "../scoreboard.dat";
    FILE *fp;
    fp= fopen(path, "r");
    struct Scoreboard res;
    fread(&res, sizeof(struct Scoreboard), 1, fp);
    fclose(fp);
    return res;
}

// Counting files in a directory
void FILECOUNT(char *path, int *cnt, char *mapName){
    // Source: https://stackoverflow.com/questions/1121383/counting-the-number-of-files-in-a-directory-using-c
    DIR * dirp;
    struct dirent * entry;
    dirp = opendir(path);
    while ((entry = readdir(dirp)) != NULL) {
        if (entry->d_type == DT_REG) {
            memset(mapName+(*cnt)*50, 0, 50);
            strcpy(mapName+(*cnt)*50, entry->d_name);
            (*cnt)++;
        }
    }
    closedir(dirp);
}

// checking if a certain file exists
int FILEEXISTS(const char *path){
    FILE *file;
    if ((file = fopen(path, "r"))){
        fclose(file);
        return 1;
    }
    return 0;
}