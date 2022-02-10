//
// Created by emad on 10/02/2022.
//

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <dirent.h>

#include "Perimeters.h"
#include "Map.h"
#include "Functions.h"
#include "FileHandling.h"
#include "Texture.h"
#include "Scoreboard.h"

#include "Menu.h"

TTF_Font *regularFont;

SDL_Texture *islandShape[MAX_SHAPE][MAX_PLAYER+1];
SDL_Texture *logoTexture[MAX_PLAYER+1];
SDL_Texture *potionTexture[POTION_CNT];
SDL_Texture *playerNameTexture[MAX_PLAYER+1];
SDL_Texture *potionNameTexture[POTION_CNT];
SDL_Texture *troopCntTexture[MAX_TROOPCNT];
int troopCntW[MAX_TROOPCNT], troopCntH[MAX_TROOPCNT];

void LOAD_TEXTURES(SDL_Renderer *sdlRenderer){
    // Islands
    for(int i=0;i<MAX_SHAPE;i++){
        for(int j=0;j<=MAX_PLAYER;j++){
            char str[40];
            memset(str, 0, 40);
            strcpy(str, "../Islands/Island");
            str[strlen(str)]='0'+(i/10);
            str[strlen(str)]='0'+(i%10);
            str[strlen(str)]='0'+j;
            strcat(str, ".bmp");
            islandShape[i][j]= getImageTexture(sdlRenderer, str);
        }
    }

    // Logos
    logoTexture[0]= getImageTexture(sdlRenderer, "../Logos/0.bmp");
    logoTexture[1]= getImageTexture(sdlRenderer, "../Logos/1.bmp");
    logoTexture[2]= getImageTexture(sdlRenderer, "../Logos/2.bmp");
    logoTexture[3]= getImageTexture(sdlRenderer, "../Logos/3.bmp");
    logoTexture[4]= getImageTexture(sdlRenderer, "../Logos/4.bmp");

    // Numbers
    regularFont= TTF_OpenFont("../Fonts/OpenSans-Regular.ttf", 15);
    for(int i=0;i<MAX_TROOPCNT;i++){
        troopCntTexture[i]= getTextTexture(sdlRenderer, TO_STRING(i), white, "../Fonts/OpenSans-Regular.ttf", 15);
        TTF_SizeText(regularFont, TO_STRING(i), &troopCntW[i], &troopCntH[i]);
    }
    TTF_CloseFont(regularFont);

    // Potions
    potionTexture[FREEZE_ID]= getImageTexture(sdlRenderer, "../Potions/FreezePotion.bmp");
    potionTexture[HASTE_ID]= getImageTexture(sdlRenderer, "../Potions/HastePotion.bmp");
    potionTexture[POACH_ID]= getImageTexture(sdlRenderer, "../Potions/PoachPotion.bmp");
    potionTexture[WARCRY_ID]= getImageTexture(sdlRenderer, "../Potions/WarcryPotion.bmp");

    // Names
    playerNameTexture[1]= getTextTexture(sdlRenderer, "You:  ", white, "../Fonts/Freebooter.ttf", 40);
    playerNameTexture[2]= getTextTexture(sdlRenderer, "Bot 1:", white, "../Fonts/Freebooter.ttf", 40);
    playerNameTexture[3]= getTextTexture(sdlRenderer, "Bot 2:", white, "../Fonts/Freebooter.ttf", 40);
    playerNameTexture[4]= getTextTexture(sdlRenderer, "Bot 3:", white, "../Fonts/Freebooter.ttf", 40);

    potionNameTexture[0]= getTextTexture(sdlRenderer, "EMPTY", white, "../Fonts/OpenSans-Regular.ttf", 60);
    potionNameTexture[WARCRY_ID]= getTextTexture(sdlRenderer, "WARCRY", white, "../Fonts/OpenSans-Regular.ttf", 60);
    potionNameTexture[POACH_ID]= getTextTexture(sdlRenderer, "POACH", white, "../Fonts/OpenSans-Regular.ttf", 60);
    potionNameTexture[HASTE_ID]= getTextTexture(sdlRenderer, "HASTE", white, "../Fonts/OpenSans-Regular.ttf", 60);
    potionNameTexture[FREEZE_ID]= getTextTexture(sdlRenderer, "FREEZE", white, "../Fonts/OpenSans-Regular.ttf", 60);
}

// displaying the map on the screen
void SHOW_MAP(SDL_Renderer *sdlRenderer, struct Map *map){
    // displaying the ocean
    putImage(sdlRenderer, "../Pics/background.bmp", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    putImage(sdlRenderer, "../Pics/wood.bmp", 0, GAME_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT-GAME_HEIGHT);

    // displaying the islands
    for(int i=0;i<map->islandCnt;i++){
        SDL_Rect islandRect = {.x=map->islandList[i].x, .y=map->islandList[i].y, .w=ISLAND_SIZE, .h=ISLAND_SIZE};
        SDL_RenderCopy(sdlRenderer, islandShape[map->islandList[i].shape][map->islandList[i].owner], NULL, &islandRect);

        SDL_Rect logoRect = {map->islandList[i].x+(ISLAND_SIZE-LOGO_SIZE)/2, map->islandList[i].y+(ISLAND_SIZE-LOGO_SIZE)/2, LOGO_SIZE, LOGO_SIZE};
        SDL_RenderCopy(sdlRenderer, logoTexture[map->islandList[i].owner], NULL, &logoRect);

        if(map->islandList[i].isSelected){
            circleColor(sdlRenderer, map->islandList[i].x+ISLAND_SIZE/2, map->islandList[i].y+ISLAND_SIZE/2, LOGO_SIZE/2, 0xff0000ff);
        }
    }

    //displaying the troops
    for(int i=0;i<map->troopCnt;i++){
        SDL_Rect troopRect = {map->troopList[i].x, map->troopList[i].y, TROOP_SIZE, TROOP_SIZE};
        SDL_RenderCopy(sdlRenderer, logoTexture[map->troopList[i].owner], NULL, &troopRect);
    }

    //displaying potions
    for(int i=0;i<map->potionCnt;i++){
        SDL_Rect potionRect = {.x=map->potionList[i].x, .y=map->potionList[i].y, .w=POTION_WIDTH, .h=POTION_HEIGHT};
        SDL_RenderCopy(sdlRenderer, potionTexture[map->potionList[i].type], NULL, &potionRect);
    }

    for(int i=0;i<map->islandCnt;i++){
        int num=map->islandList[i].troopsCount;
        if(num>=map->islandList[i].capacity){
            filledCircleColor(sdlRenderer, map->islandList[i].x+ISLAND_SIZE/2, map->islandList[i].y+ISLAND_SIZE/2, 10, 0xff0000ff);
        }
        SDL_Rect scoreRect={map->islandList[i].x+(ISLAND_SIZE-troopCntW[num])/2, map->islandList[i].y+(ISLAND_SIZE-troopCntH[num])/2, troopCntW[num], troopCntH[num]};
        SDL_RenderCopy(sdlRenderer, troopCntTexture[num], NULL, &scoreRect);
    }
}

void SHOW_STATS(SDL_Renderer *sdlRenderer, struct Map *map){
    for(int i=1;i<=map->playerCnt;i++){
        SDL_Rect nameRect={75+250*(i-1), GAME_HEIGHT+25, 75, 40};
        SDL_RenderCopy(sdlRenderer, playerNameTexture[i], NULL, &nameRect);
        boxColor(sdlRenderer, 100+250*(i-1), GAME_HEIGHT+125, 100+250*(i-1)+map->playerList[i].potionLeft/6, GAME_HEIGHT+135, potionColor[map->playerList[i].potion]);
        SDL_Rect potionRect={100+250*(i-1), GAME_HEIGHT+75, 100, 50};
        SDL_RenderCopy(sdlRenderer, potionNameTexture[map->playerList[i].potion], NULL, &potionRect);
    }
}

void LOADING_SCREEN(SDL_Renderer *sdlRenderer, int *state, SDL_bool *shallExit){
    boxColor(sdlRenderer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0xff000000);
    putImage(sdlRenderer, "../Pics/pain.bmp", 700, 400, 100, 100);
    putTextMid(sdlRenderer, "PAIN STUDIOS", white, "../Fonts/Freebooter.ttf", 50, 510);
    putTextMid(sdlRenderer, "presents", white, "../Fonts/Freebooter.ttf", 25, 550);
    SDL_RenderPresent(sdlRenderer);
    SDL_Delay(2000);

    putImage(sdlRenderer, "../Pics/pirate1.bmp", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    putTextMid(sdlRenderer, "Islands.io", black, "../Fonts/Primitive.ttf", 150, 300);
    putTextMid(sdlRenderer, "by Emad EmamJomeh", black, "../Fonts/Primitive.ttf", 40, 600);
    putTextMid(sdlRenderer, "FOP 1400-1", black, "../Fonts/Primitive.ttf", 30, 700);
    SDL_RenderPresent(sdlRenderer);
    SDL_Delay(3000);

    *state=8;
    SDL_Event sdlEvent;
    while(SDL_PollEvent(&sdlEvent)){
        if(sdlEvent.type==SDL_QUIT){
            *shallExit=SDL_TRUE;
            break;
        }
    }
}

void MENU(SDL_Renderer *sdlRenderer, int *state, SDL_bool *shallExit){
    putImage(sdlRenderer, "../Pics/pirate1.bmp", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    putImage(sdlRenderer, "../Pics/paperver6.bmp", 400, 50, 700, 900);

    putText(sdlRenderer, "Main Menu", black, "../Fonts/Freebooter.ttf", 110, SCREEN_WIDTH/2-200, 200);

    // displaying buttons
    const int buttonW=500, buttonH=100;
    const int buttonY[4]={ 375, 500, 625, 750};
    for(int i=0;i<4;i++){
        //roundedBoxColor(sdlRenderer, (SCREEN_WIDTH-buttonW)/2, buttonY[i], (SCREEN_WIDTH+buttonW)/2, buttonY[i]+buttonH, 10,0xff808080);
        putImage(sdlRenderer, "../Pics/wood.bmp", (SCREEN_WIDTH-buttonW)/2, buttonY[i], buttonW, buttonH);
    }
    putTextMid(sdlRenderer, "New Game", white, "../Fonts/Freebooter.ttf", 60, buttonY[0]+20);
    putTextMid(sdlRenderer, "Continue Game", white, "../Fonts/Freebooter.ttf", 60, buttonY[1]+20);
    putTextMid(sdlRenderer, "Scoreboard", white, "../Fonts/Freebooter.ttf", 60, buttonY[2]+20);
    putTextMid(sdlRenderer, "Rename", white, "../Fonts/Freebooter.ttf", 60, buttonY[3]+20);


    SDL_RenderPresent(sdlRenderer);
    SDL_Delay(1000/FPS);
    SDL_Event sdlEvent;
    while(SDL_PollEvent(&sdlEvent)){
        if(sdlEvent.type==SDL_QUIT){
            *shallExit=SDL_TRUE;
            break;
        }
        if(sdlEvent.type==SDL_MOUSEBUTTONDOWN){
            if(COLLIDE(sdlEvent.button.x, sdlEvent.button.y, 1, 1, (SCREEN_WIDTH-buttonW)/2, buttonY[0], buttonW, buttonH)){
                *state = 4;
            }
            if(COLLIDE(sdlEvent.button.x, sdlEvent.button.y, 1, 1, (SCREEN_WIDTH-buttonW)/2, buttonY[1], buttonW, buttonH)){
                *state = 6;
            }
            if(COLLIDE(sdlEvent.button.x, sdlEvent.button.y, 1, 1, (SCREEN_WIDTH-buttonW)/2, buttonY[2], buttonW, buttonH)){
                *state = 7;
            }
            if(COLLIDE(sdlEvent.button.x, sdlEvent.button.y, 1, 1, (SCREEN_WIDTH-buttonW)/2, buttonY[3], buttonW, buttonH)){
                *state = 8;
            }
        }
    }
}

void GAME_PLAYING(SDL_Renderer *sdlRenderer, int *state, SDL_bool *shallExit, char *userName, struct Map *map){
    // Updating everything
    int isOver=MAP_UPDATE(map);

    SHOW_MAP(sdlRenderer, map);
    SHOW_STATS(sdlRenderer, map);

    if(isOver==-1){ // User lost
        boxColor(sdlRenderer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0xa0000000);
        putTextMid(sdlRenderer, "YOU LOST", red, "../Fonts/Bacon.otf", 100, 300);
        putTextMid(sdlRenderer, "You will be deducted 10 points!", red, "../Fonts/Freebooter.ttf", 50, 600);

        struct Scoreboard sb=LOAD_SCOREBOARD();
        ADD_TO_SCOREBOARD(&sb, userName, -LOSE_SCORE);
        SAVE_SCOREBOARD(sb);
        *state=1;
    }
    if(isOver==1){ // User won
        boxColor(sdlRenderer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0xa0000000);
        putTextMid(sdlRenderer, "YOU WON", green, "../Fonts/HappyKids.ttf", 100, 300);
        putTextMid(sdlRenderer, "You will receive 40 Extra points!", green, "../Fonts/Freebooter.ttf", 50, 600);

        struct Scoreboard sb=LOAD_SCOREBOARD();
        ADD_TO_SCOREBOARD(&sb, userName, WIN_SCORE);
        SAVE_SCOREBOARD(sb);
        *state=1;
    }

    putImage(sdlRenderer, "../Buttons/pause.bmp", 1200, 850, 100, 100);

    SDL_RenderPresent(sdlRenderer);
    SDL_Delay(1000 / FPS - 4);
    SDL_Event ev;
    while(SDL_PollEvent(&ev)){
        if(ev.type==SDL_QUIT){
            *shallExit=SDL_TRUE;
            break;
        }
        if(ev.type==SDL_MOUSEBUTTONDOWN){
            CLICKED(ev.button.x,ev.button.y,map);
            if( COLLIDE(ev.button.x, ev.button.y, 1, 1, 1200, 850, 100, 100)){
                *state=3;
            }
        }
    }

    if(isOver!=0) SDL_Delay(5000);
}

void GAME_PAUSED(SDL_Renderer *sdlRenderer, int *state, SDL_bool *shallExit, struct Map *map){
    SHOW_MAP(sdlRenderer, map);
    boxColor(sdlRenderer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0xc0000000);
    putImage(sdlRenderer, "../Pics/wood.bmp", 0, GAME_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT-GAME_HEIGHT);

    putTextMid(sdlRenderer, "Game paused", white, "../Fonts/Primitive.ttf", 80, 300);

    putImage(sdlRenderer, "../Buttons/play.bmp", 1200, 850, 100, 100);
    putImage(sdlRenderer, "../Buttons/save.bmp", 700, 850, 100, 100);
    putImage(sdlRenderer, "../Buttons/home.bmp", 200, 850, 100, 100);

    SDL_RenderPresent(sdlRenderer);
    SDL_Delay(1000/FPS);
    SDL_Event ev;
    while(SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT) {
            *shallExit = SDL_TRUE;
            break;
        }
        if(ev.type == SDL_MOUSEBUTTONDOWN){
            if(COLLIDE(ev.button.x, ev.button.y, 0, 0, 1200, 850, 100, 100)){
                *state=2;
            }
            if(COLLIDE(ev.button.x, ev.button.y, 0, 0, 700, 850, 100, 100)){
                SAVE_GAME(*map);
            }
            if(COLLIDE(ev.button.x, ev.button.y, 0, 0, 200, 850, 100, 100)){
                *state=1;
            }
        }
    }
}

void NEW_GAME(SDL_Renderer *sdlRenderer, int *state, SDL_bool *shallExit, const int *mapCnt, char mapName[MAX_MAPS][50], int PlayerCntNow, int IslandCntNow, struct Map *map){
    putImage(sdlRenderer, "../Pics/pirate1.bmp", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    putTextMid(sdlRenderer, "Choose a Map", black, "../Fonts/Freebooter.ttf", 100, 100);

    int buttonW=800, buttonH=80;
    int buttonX[*mapCnt], buttonY[*mapCnt];
    for(int i=0;i<*mapCnt;i++){
        buttonX[i]=(SCREEN_WIDTH-800)/2;
        buttonY[i]=200+i*100;
        putImage(sdlRenderer, "../Pics/wood.bmp", buttonX[i], buttonY[i], buttonW, buttonH);

        char name[50];
        memset(name, 0, 50);
        strncpy(name, mapName[i], strlen(mapName[i])-4);
        putTextMid(sdlRenderer, name, white, "../Fonts/Freebooter.ttf", 40, buttonY[i]+(buttonH-50)/2);
    }

    putImage(sdlRenderer, "../Buttons/home.bmp", 1200, 850, 100, 100);
    putImage(sdlRenderer, "../Buttons/plus.bmp", 1200, 725, 100, 100);

    SDL_RenderPresent(sdlRenderer);
    SDL_Delay(1000/FPS);
    SDL_Event ev;
    while(SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT) {
            *shallExit = SDL_TRUE;
            break;
        }
        if(ev.type == SDL_MOUSEBUTTONDOWN){
            for(int i=0;i<*mapCnt;i++){ // checking if a map is chosen
                if(COLLIDE(ev.button.x, ev.button.y, 1, 1, buttonX[i], buttonY[i], buttonW, buttonH)){
                    *map=LOAD_MAP(mapName[i]);
                    *state=2;
                }
            }
            if(COLLIDE(ev.button.x, ev.button.y, 1, 1, 1200, 725, 100, 100)){
                *state=5;
                *map=MAP_GENERATOR(IslandCntNow, PlayerCntNow);
            }
            if( COLLIDE(ev.button.x, ev.button.y, 1, 1, 1200, 850, 100, 100)){
                *state=1;
            }
        }
    }
}

void NEW_MAP(SDL_Renderer *sdlRenderer, int *state, SDL_bool *shallExit, int *IslandCntNow, int *PlayerCntNow, char *mapID,int *mapCnt, char *mapName, struct Map *map){
    SHOW_MAP(sdlRenderer, map);

    putText(sdlRenderer, "Island Count:", white, "../Fonts/Freebooter.ttf", 40, 75, 850);
    putText(sdlRenderer, TO_STRING(*IslandCntNow), white, "../Fonts/Freebooter.ttf", 40, 75, 900);
    putImage(sdlRenderer, "../Buttons/up.bmp", 275, 840, 50, 50);
    putImage(sdlRenderer, "../Buttons/down.bmp", 275, 900, 50, 50);

    putText(sdlRenderer, "Player Count:", white, "../Fonts/Freebooter.ttf", 40, 425, 850);
    putText(sdlRenderer, TO_STRING(*PlayerCntNow), white, "../Fonts/Freebooter.ttf", 40, 425, 900);
    putImage(sdlRenderer, "../Buttons/up.bmp", 625, 840, 50, 50);
    putImage(sdlRenderer, "../Buttons/down.bmp", 625, 900, 50, 50);

    TTF_Font *font= TTF_OpenFont("../Fonts/Freebooter.ttf", 50);
    int w,h;
    TTF_SizeText(font, mapID, &w, &h);
    SDL_Rect textRect={(SCREEN_WIDTH-w)/2+200, 900-h/2, w, h};
    boxColor(sdlRenderer, (SCREEN_WIDTH-w)/2+200, 900-h/2, (SCREEN_WIDTH+w)/2+200, 900+h/2, 0x80000000);
    SDL_Texture *text= getTextTexture(sdlRenderer, mapID, white, "../Fonts/Freebooter.ttf", 50);
    SDL_RenderCopy(sdlRenderer, text, NULL, &textRect);
    SDL_DestroyTexture(text);
    TTF_CloseFont(font);

    putImage(sdlRenderer, "../Buttons/confirm.bmp", 1200, 850, 100, 100);
    putImage(sdlRenderer, "../Buttons/back.bmp", 1350, 850, 100, 100);

    SDL_RenderPresent(sdlRenderer);
    SDL_Delay(1000/FPS);
    SDL_Event ev;
    while(SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT) {
            *shallExit = SDL_TRUE;
            break;
        }
        else if(ev.type == SDL_MOUSEBUTTONDOWN){
            if( COLLIDE(ev.button.x, ev.button.y, 0, 0, 275, 840, 50, 50)){
                *IslandCntNow=MIN(*IslandCntNow+1, 20);
            }
            if( COLLIDE(ev.button.x, ev.button.y, 0, 0, 275, 900, 50, 50)){
                *IslandCntNow=MAX(*IslandCntNow-1, 10);
            }
            if( COLLIDE(ev.button.x, ev.button.y, 0, 0, 625, 840, 50, 50)){
                *PlayerCntNow=MIN(*PlayerCntNow+1, 4);
            }
            if( COLLIDE(ev.button.x, ev.button.y, 0, 0, 625, 900, 50, 50)){
                *PlayerCntNow=MAX(*PlayerCntNow-1, 2);
            }
            if( COLLIDE(ev.button.x, ev.button.y, 0, 0, 1200, 850, 100, 100)){
                *state=4;
                SAVE_MAP(*map, mapID);
                memset((mapName+(*mapCnt)*50), 0, 50);
                strcpy((mapName+(*mapCnt)*50), mapID);
                strcat((mapName+(*mapCnt)*50), ".dat");
                (*mapCnt)++;
                memset(mapID, 0, 50);
                strcpy(mapID, "Enter name");
            }
            if( COLLIDE(ev.button.x, ev.button.y, 0, 0, 1350, 850, 100, 100)){
                *state=4;
                strcpy(mapID, "Enter name");
            }
            do{
                *map= MAP_GENERATOR(*IslandCntNow, *PlayerCntNow);
            }while(!MAP_CHECK(*map));

        }
        else if(ev.type==SDL_TEXTINPUT){
            strcat(mapID, ev.text.text);
        }
        else if(ev.type==SDL_KEYDOWN && ev.key.keysym.sym==SDLK_BACKSPACE){
            mapID[strlen(mapID)-1]=0;
        }
        else if(ev.type==SDL_KEYDOWN && ev.key.keysym.sym==SDLK_RETURN){
            *state=4;
            SAVE_MAP(*map, mapID);
            memset((mapName+(*mapCnt)*50), 0, 50);
            strcpy((mapName+(*mapCnt)*50), mapID);
            strcat((mapName+(*mapCnt)*50), ".dat");
            (*mapCnt)++;
            memset(mapID, 0, 50);
            strcpy(mapID, "Enter name");
        }
    }
}

void CONTINUE(SDL_Renderer *sdlRenderer, int *state, SDL_bool *shallExit, struct Map *map){
    putImage(sdlRenderer, "../Pics/pirate1.bmp", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    if(FILEEXISTS("../lastGame.dat")==0){
        putTextMid(sdlRenderer, "There is no Saved Game!", black, "../Fonts/Primitive.ttf", 80, 400);
        *state=1;
    }
    else{
        *map= LOAD_GAME();
        *state=2;
    }

    SDL_RenderPresent(sdlRenderer);
    SDL_Delay(1000/FPS);
    if(FILEEXISTS("../lastGame.dat")==0) SDL_Delay(1500);
    SDL_Event sdlEvent;
    while(SDL_PollEvent(&sdlEvent)) {
        if (sdlEvent.type == SDL_QUIT) {
            *shallExit = SDL_TRUE;
            break;
        }
    }
}

void SCOREBOARD(SDL_Renderer *sdlRenderer, int *state, SDL_bool *shallExit){
    putImage(sdlRenderer, "../Pics/pirate1.bmp", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    putImage(sdlRenderer, "../Pics/paperver5.bmp", 300, 0, 900, 900);

    putTextMid(sdlRenderer, "ScoreBoard", black, "../Fonts/Primitive.ttf", 80, 160);
    struct Scoreboard sb=LOAD_SCOREBOARD();
    for(int i=0;i<MIN(sb.userCnt, 10);i++){
        putText(sdlRenderer, sb.nameList[i], black, "../Fonts/Primitive.ttf", 40, 450, 300+i*50);
        putText(sdlRenderer, TO_STRING(sb.scoreList[i]), black, "../Fonts/Primitive.ttf", 40, 1000, 300+i*50);
    }

    putImage(sdlRenderer, "../Buttons/home.bmp", 1300, 850, 100, 100);

    SDL_RenderPresent(sdlRenderer);
    SDL_Delay(1000/FPS);
    SDL_Event ev;
    while(SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT) {
            *shallExit = SDL_TRUE;
            break;
        }
        if(ev.type==SDL_MOUSEBUTTONDOWN){
            if( COLLIDE(ev.button.x, ev.button.y, 1, 1, 1300, 850, 100, 100)){
                *state=1;
            }
        }
    }
}

void ENTER_NAME(SDL_Renderer *sdlRenderer, int *state, SDL_bool *shallExit, char *userName){
    putImage(sdlRenderer, "../Pics/pirate1.bmp", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    putImage(sdlRenderer, "../Pics/paperhor1.bmp", 250, 150, 1000, 700);
    putText(sdlRenderer, "Enter Your Name:", black, "../Fonts/Freebooter.ttf", 100, SCREEN_WIDTH/2-300, 325);

    TTF_Font *font= TTF_OpenFont("../Fonts/Freebooter.ttf", 60);
    int w,h;
    TTF_SizeText(font, userName, &w, &h);
    SDL_Rect textRect={(SCREEN_WIDTH-w)/2, (SCREEN_HEIGHT-h)/2, w, h};
    boxColor(sdlRenderer, (SCREEN_WIDTH-w)/2, (SCREEN_HEIGHT-h)/2, (SCREEN_WIDTH+w)/2, (SCREEN_HEIGHT+h)/2, 0x8098a558);
    SDL_Texture *text= getTextTexture(sdlRenderer, userName, black, "../Fonts/Freebooter.ttf", 60);
    SDL_RenderCopy(sdlRenderer, text, NULL, &textRect);
    SDL_DestroyTexture(text);
    TTF_CloseFont(font);

    SDL_RenderPresent(sdlRenderer);
    SDL_Delay(1000/FPS);
    SDL_StartTextInput();
    SDL_Event ev;
    while(SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT) {
            *shallExit = SDL_TRUE;
            break;
        }
        else if(ev.type==SDL_TEXTINPUT){
            strcat(userName, ev.text.text);
        }
        else if(ev.type==SDL_KEYDOWN && ev.key.keysym.sym==SDLK_BACKSPACE){
            userName[strlen(userName)-1]=0;
        }
        else if(ev.type==SDL_KEYDOWN && ev.key.keysym.sym==SDLK_RETURN){
            *state=1;
        }
    }
}
