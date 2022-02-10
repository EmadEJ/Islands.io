//
// Created by emad on 02/02/2022.
//

#ifndef TEST_GRAPHICS_PERIMETERS_H
#define TEST_GRAPHICS_PERIMETERS_H

#include <SDL2/SDL.h>

#define ISLAND_SIZE 150
#define LOGO_SIZE 50
#define TROOP_SIZE 15
#define POTION_WIDTH 35
#define POTION_HEIGHT 50

#define MAX_ISLANDS 25
#define MAX_TROOPS 2000
#define MAX_CAMPAIGN 20
#define MAX_SHAPE 20
#define MAX_PLAYER 4
#define MAX_FRAME 600000
#define MAX_POTION 3
#define MAX_TROOPCNT 200
#define MAX_USER 20
#define MAX_MAPS 20

#define DEFAULT_TROOPS 10
#define MAX_CAPACITY 50
#define MIN_CAPACITY 30
#define POTION_CNT 5

#define USERID 1

// default perimeters
#define FRAME_PER_PROD 80
#define FRAME_PER_OUT 10
#define TROOP_SPEED 3
#define POTION_CHANCE 300

// Potion perimeters
#define FREEZE_ID 1
#define HASTE_ID 2
#define POACH_ID 3
#define WARCRY_ID 4

// Scoring
#define WIN_SCORE 40
#define LOSE_SCORE 10

// Potion length
static const int POTION_LEN[5]= {0, 300, 600, 300, 600};

// Screen info
#define GAME_WIDTH 1500
#define GAME_HEIGHT 800
#define SCREEN_WIDTH 1500
#define SCREEN_HEIGHT 1000
#define FPS 60

// colors
static const SDL_Color black={0, 0, 0}, white={255, 255, 255 }, red={255, 0, 0}, green={0, 255, 0};
static const unsigned int potionColor[5]={0x00000000, 0xffffd8ff, 0xff48ff00, 0xffff00d0, 0xff00eaff};

static const int INF=1000*1000*1000;

#endif //TEST_GRAPHICS_PERIMETERS_H
