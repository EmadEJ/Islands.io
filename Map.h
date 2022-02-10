//
// Created by Emad on 31/01/2022.
//
#pragma once

#ifndef TEST_GRAPHICS_MAP_H
#define TEST_GRAPHICS_MAP_H

#include "Functions.h"
#include "Perimeters.h"

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
    int frame;
};

// holds the data about a player
struct Player{
    int potion;
    int potionLeft;
    int islandCnt;
    int troopCnt;
};

struct Potion{
    int x,y;
    int type;
};

// holds the data about the map (the whole game actually)
struct Map{
    int frameNo;
    int islandCnt;
    struct Island islandList[MAX_ISLANDS];
    int selectedIsland;
    int playerCnt;
    struct Player playerList[MAX_PLAYER+1];
    int troopCnt;
    struct Troop troopList[MAX_TROOPS];
    int campaignCnt;
    struct Campaign campaignList[MAX_CAMPAIGN];
    int potionCnt;
    struct Potion potionList[MAX_POTION];
};

// checking if islands intersect
int ISLAND_COLLIDE(int x1, int y1, int x2,int y2);

// generating a random map based
struct Map MAP_GENERATOR(int islandCnt, int playerCnt);
// A few map based functions

void ADD_TROOP(struct Troop t, struct Map *map);

void DESTROY_TROOP(int ind, struct Map *map);

void ADD_CAMPAIGN(struct Campaign c, struct Map *map);

void DESTROY_CAMPAIGN(int ind, struct Map *map);

void ADD_POTION(struct Potion p, struct Map *map);

void DESTROY_POTION(int ind, struct Map *map);

void NEW_CAMPAIGN(int st,int fin, struct Map *map);

////////////////////////////////// Game based functions

// handling clicking islands during the game
void CLICKED(int x,int y, struct Map *map);

///////// Sub-functions of the "MAP_UPDATE()"
void MOVE_TROOPS(int frozen, struct Map *map);

struct Campaign HANDLE_CAMPAIGN(struct Campaign c, struct Map *map);

void PRODUCE_TROOPS(struct Map *map);

void GENERATE_POTION(struct Map *map);

void COLLISION_CHECK(struct Map *map);

void PLAYER_UPDATE(struct Map *map);

void AI(int id, struct Map *map);

// updating the necessary parts of the map for each frame (movement, potion generation, troop production, islands state, Collisions, ...)
int MAP_UPDATE(struct Map *map);

#endif //TEST_GRAPHICS_MAP_H
