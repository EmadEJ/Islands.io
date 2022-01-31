//
// Created by Emad on 31/01/2022.
//

#ifndef TEST_GRAPHICS_MAP_H
#define TEST_GRAPHICS_MAP_H

#include "Functions.h"

#define ISLAND_SIZE 150
#define LOGO_SIZE 50
#define TROOP_SIZE 20
#define MAX_ISLANDS 25
#define MAX_TROOPS 2000
#define MAX_CAMPAIGN 20
#define MAX_SHAPE 1
#define MAX_PLAYER 4
#define MAX_FRAME 600000

#define SCREEN_WIDTH 1500
#define SCREEN_HEIGHT 1000
#define FPS 60

// holds the data for an island
struct Island{
    int troopsCount,capacity;
    int owner;
    int shape;
    int isSelected;
    int x,y;
};

// holds the data for a troop
struct Troop{
    float x,y;
    float xSpeed,ySpeed;
    int owner;
    int dest;
};

// holds the data for a campaign (*Lashkar-Keshi)
struct Campaign{
    int xStart,yStart;
    int xEnd,yEnd;
    int count;
    int dest;
    int owner;
};

// holds the data about a player
struct Player{
    int potion;
    int potionLeft;
    int islandCnt;
    int troopCnt;
};

// holds the data about the map (the whole game actually)
struct Map{
    int islandCnt;
    struct Island islandList[MAX_ISLANDS];
    int selectedIsland;
    int playerCnt;
    struct Player playerList[MAX_PLAYER];
    int troopCnt;
    struct Troop troopList[MAX_TROOPS];
    int campaignCnt;
    struct Campaign campaignList[MAX_CAMPAIGN];
};

// checking if islands intersect
int ISLAND_COLLIDE(int x1, int y1, int x2,int y2){
    int dis=(ISLAND_SIZE+LOGO_SIZE)/2;
    if( (x1<x2+dis && x1>x2-dis) && (y1<y2+dis && y1>y2-dis) ) return 1;
    return 0;
}

// generating a random map based
struct Map MAP_GENERATOR(int islandCnt, int playerCnt){
    struct Map res;
    res.islandCnt=islandCnt;
    for(int i=0;i<islandCnt;i++){

        // randomizing coords
        int x,y;
        int isok=0;
        while(!isok){
            isok=1;
            x=RAND(0,SCREEN_WIDTH-ISLAND_SIZE);
            y=RAND(0,SCREEN_HEIGHT-ISLAND_SIZE);
            for(int j=0;j<i;j++) {
                if (ISLAND_COLLIDE(x, y, res.islandList[j].x, res.islandList[j].y)) {
                    isok = 0;
                    break;
                }
            }
        }

        res.islandList[i].shape= RAND(0,MAX_SHAPE);
        res.islandList[i].x=x;
        res.islandList[i].y=y;
        res.islandList[i].isSelected=0;
        res.islandList[i].owner=0;
        res.islandList[i].capacity=50;
        res.islandList[i].troopsCount=0;
        if(i<playerCnt){
            res.islandList[i].owner= i+1;
        }
    }

    res.playerCnt=playerCnt;
    res.troopCnt=0;
    res.campaignCnt=0;
    res.selectedIsland=-1;
    return res;
};

#endif //TEST_GRAPHICS_MAP_H
