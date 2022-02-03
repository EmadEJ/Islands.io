//
// Created by emad on 02/02/2022.
//

#ifndef TEST_GRAPHICS_FILEHANDLING_H
#define TEST_GRAPHICS_FILEHANDLING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "Map.h"

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

struct Map LOAD_MAP(char *name){
    char path[50];
    memset(path, 0, 50);
    strcpy(path, "../Maps/");
    strcat(path, name);
    strcat(path, ".dat");
    FILE *fp;
    fp= fopen(path, "r");
    struct Map res;
    fread(&res, sizeof(struct Map), 1, fp);
    fclose(fp);
    return res;
}

// Saving and loading the last game
void SAVE_GAME(struct Map inp){
    const char *path= "../lastGame.dat";
    FILE *fp;
    fp= fopen(path, "w");
    fwrite(&inp, sizeof(struct Map), 1, fp);
    fclose(fp);
}

struct Map LOAD_GAME(){
    const char *path= "../lastGame.dat";
    FILE *fp;
    fp= fopen(path, "r");
    struct Map res;
    fread(&res, sizeof(struct Map), 1, fp);
    fclose(fp);
    return res;
}

// Counting files in a directory
int FILECOUNT(char *path){
    // Source: https://stackoverflow.com/questions/1121383/counting-the-number-of-files-in-a-directory-using-c
    int cnt = 0;
    DIR * dirp;
    struct dirent * entry;
    dirp = opendir(path);
    while ((entry = readdir(dirp)) != NULL) {
        if (entry->d_type == DT_REG) {
            cnt++;
        }
    }
    closedir(dirp);
    return cnt;
}

int FILEEXISTS(const char *path){
    FILE *file;
    if ((file = fopen(path, "r"))){
        fclose(file);
        return 1;
    }
    return 0;
}

#endif //TEST_GRAPHICS_FILEHANDLING_H
