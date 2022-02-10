//
// Created by emad on 10/02/2022.
//

/*
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
*/
#include "Menu.h"
/*
void LOADING_SCREEN(SDL_Renderer *sdlRenderer, int *state, SDL_bool *shallExit){
    putImage(sdlRenderer, "../Pics/pirate1.bmp", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    putTextMid(sdlRenderer, "Islands.io", black, "../Fonts/Primitive.ttf", 150, 300);
    putTextMid(sdlRenderer, "by Emad EmamJomeh", black, "../Fonts/Primitive.ttf", 40, 600);
    putTextMid(sdlRenderer, "FOP 1400-1", black, "../Fonts/Primitive.ttf", 30, 700);

    SDL_RenderPresent(sdlRenderer);
    SDL_Delay(1000);
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
    const int buttonY[3]={ 400, 525, 650};
    for(int i=0;i<3;i++){
        //roundedBoxColor(sdlRenderer, (SCREEN_WIDTH-buttonW)/2, buttonY[i], (SCREEN_WIDTH+buttonW)/2, buttonY[i]+buttonH, 10,0xff808080);
        putImage(sdlRenderer, "../Pics/wood.bmp", (SCREEN_WIDTH-buttonW)/2, buttonY[i], buttonW, buttonH);
    }
    putTextMid(sdlRenderer, "New Game", white, "../Fonts/Freebooter.ttf", 60, buttonY[0]+20);
    putTextMid(sdlRenderer, "Continue Game", white, "../Fonts/Freebooter.ttf", 60, buttonY[1]+20);
    putTextMid(sdlRenderer, "Scoreboard", white, "../Fonts/Freebooter.ttf", 60, buttonY[2]+20);


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
        }
    }
}

void GAME_PAUSED(SDL_Renderer *sdlRenderer, int *state, SDL_bool *shallExit){
    SHOW_MAP(sdlRenderer);
    boxColor(sdlRenderer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0xc0000000);
    putImage(sdlRenderer, "../Pics/wood.bmp", 0, GAME_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT-GAME_HEIGHT);

    SDL_Texture *title= getTextTexture(sdlRenderer, "Game paused", white, "../Fonts/OpenSans-Bold.ttf", 100);
    SDL_Rect titleRect= {.x=(SCREEN_WIDTH-300)/2, .y=300, .w=300, .h=100};
    SDL_RenderCopy(sdlRenderer, title, NULL, &titleRect);
    SDL_DestroyTexture(title);

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
                SAVE_GAME(map);
            }
            if(COLLIDE(ev.button.x, ev.button.y, 0, 0, 200, 850, 100, 100)){
                *state=1;
            }
        }
    }
}

void NEW_GAME(SDL_Renderer *sdlRenderer, int *state, SDL_bool *shallExit, const int *mapCnt, char mapName[MAX_MAPS][50], int PlayerCntNow, int IslandCntNow){
    putImage(sdlRenderer, "../Pics/pirate1.bmp", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    putTextMid(sdlRenderer, "Choose a Map", black, "../Fonts/Freebooter.ttf", 100, 100);

    int buttonW=800, buttonH=80;
    int buttonX[*mapCnt], buttonY[*mapCnt];
    for(int i=0;i<*mapCnt;i++){
        buttonX[i]=(SCREEN_WIDTH-800)/2;
        buttonY[i]=200+i*100;
        putImage(sdlRenderer, "../Pics/wood.bmp", buttonX[i], buttonY[i], buttonW, buttonH);

        char name[50];
        strncpy(name, mapName[i], strlen(mapName[i])-4);
        putText(sdlRenderer, mapName[i], white, "../Fonts/Freebooter.ttf", 40, buttonX[i]+50, buttonY[i]+(buttonH-50)/2);
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
                    map= LOAD_MAP(mapName[i]);
                    *state=2;
                }
            }
            if(COLLIDE(ev.button.x, ev.button.y, 1, 1, 1200, 725, 100, 100)){
                *state=5;
                map= MAP_GENERATOR(IslandCntNow, PlayerCntNow);
            }
            if( COLLIDE(ev.button.x, ev.button.y, 1, 1, 1200, 850, 100, 100)){
                *state=1;
            }
        }
    }
}

void CONTINUE(SDL_Renderer *sdlRenderer, int *state, SDL_bool *shallExit){

    if(FILEEXISTS("../lastGame.dat")==0){
        printf("there is no saved game\n");
        *state=1;
    }
    else{
        map= LOAD_GAME();
        *state=2;
    }

    SDL_RenderPresent(sdlRenderer);
    SDL_Delay(1000/FPS);
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

    putTextMid(sdlRenderer, "ScoreBoard", black, "../Fonts/Primitive.ttf", 80, 150);
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
*/