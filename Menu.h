//
// Created by emad on 10/02/2022.
//

#ifndef TEST_GRAPHICS_MENU_H
#define TEST_GRAPHICS_MENU_H

void LOAD_TEXTURES(SDL_Renderer *sdlRenderer);

void DESTROY_TEXTURES(SDL_Renderer *sdlRenderer);

void SHOW_MAP(SDL_Renderer *sdlRenderer, struct Map *map);

void SHOW_STATS(SDL_Renderer *sdlRenderer, struct Map *map);

void LOADING_SCREEN(SDL_Renderer *sdlRenderer, int *state, SDL_bool *shallExit);

void MENU(SDL_Renderer *sdlRenderer, int *state, SDL_bool *shallExit);

void GAME_PLAYING(SDL_Renderer *sdlRenderer, int *state, SDL_bool *shallExit, char *userName, struct Map *map);

void GAME_PAUSED(SDL_Renderer *sdlRenderer, int *state, SDL_bool *shallExit, struct Map *map);

void NEW_GAME(SDL_Renderer *sdlRenderer, int *state, SDL_bool *shallExit, const int *mapCnt, char mapName[MAX_MAPS][50], int PlayerCntNow, int IslandCntNow, struct Map *map);

void NEW_MAP(SDL_Renderer *sdlRenderer, int *state, SDL_bool *shallExit, int *IslandCntNow, int *PlayerCntNow, char *mapID,int *mapCnt, char *mapName, struct Map *map);

void CONTINUE(SDL_Renderer *sdlRenderer, int *state, SDL_bool *shallExit, struct Map *map);

void SCOREBOARD(SDL_Renderer *sdlRenderer, int *state, SDL_bool *shallExit);

void ENTER_NAME(SDL_Renderer *sdlRenderer, int *state, SDL_bool *shallExit, char *userName);

#endif //TEST_GRAPHICS_MENU_H
