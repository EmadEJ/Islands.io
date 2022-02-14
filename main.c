//
// Created by Emad on 21/01/2022.
//

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "lib/Perimeters.h"
#include "lib/Map.h"
#include "lib/FileHandling.h"
#include "lib/Menu.h"

int main() {
    ///////// PREP
    srand(time(NULL));
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }
    if(TTF_Init()<0){
        printf("Error Loading TTF\n");
        return 0;
    }
    int mapCnt = 0;
    char mapName[MAX_MAPS][50];
    FILECOUNT("../Maps", &mapCnt, (char *)mapName);

    ///////////// main
    SDL_Window *sdlWindow = SDL_CreateWindow("Islands.io", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH,SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
    SDL_Renderer *sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

    LOAD_TEXTURES(sdlRenderer);

    struct Map map;

    /* state:
     * 0 -> loading screen
     * 1 -> menu
     * 2 -> game playing
     * 3 -> game paused
     * 4 -> new game
     * 5 -> new map
     * 6 -> continue
     * 7 -> scoreboard
     * 8 -> entering name
     */
    int state=0;

    int IslandCntNow=15, PlayerCntNow=3;
    char userName[50], mapID[50];
    memset(userName, 0, 50);
    memset(mapID, 0, 50);
    strcpy(mapID, "Enter name");

    SDL_bool shallExit = SDL_FALSE;
    // Game Loop
    while(shallExit == SDL_FALSE){
        SDL_SetRenderDrawColor(sdlRenderer, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderClear(sdlRenderer);

        if(state==0){
            LOADING_SCREEN(sdlRenderer, &state, &shallExit);
        }
        else if(state==1){
            MENU(sdlRenderer, &state, &shallExit);
        }
        else if(state==2){
            GAME_PLAYING(sdlRenderer, &state, &shallExit, userName, &map);
        }
        else if(state==3){
            GAME_PAUSED(sdlRenderer, &state, &shallExit, &map);
        }
        else if(state==4){
            NEW_GAME(sdlRenderer, &state, &shallExit, &mapCnt, mapName, PlayerCntNow, IslandCntNow, &map);
        }
        else if(state==5){
            NEW_MAP(sdlRenderer, &state, &shallExit, &IslandCntNow, &PlayerCntNow, mapID, &mapCnt, (char *)mapName, &map);
        }
        else if(state==6){
            CONTINUE(sdlRenderer, &state, &shallExit, &map);
        }
        else if(state==7){
            SCOREBOARD(sdlRenderer, &state, &shallExit);
        }
        else if(state==8){
            ENTER_NAME(sdlRenderer, &state, &shallExit, userName);
        }
    }

    DESTROY_TEXTURES(sdlRenderer);
    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();
    TTF_Quit();
    return 0;
}
