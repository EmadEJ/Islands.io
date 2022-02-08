//
// Created by Emad on 21/01/2022.
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

#define MAX_TROOPCNT 200

struct Map map;

TTF_Font *regularFont;

SDL_Texture *islandShape[MAX_SHAPE][MAX_PLAYER+1];
SDL_Texture *logo;
SDL_Texture *troopCntTexture[MAX_TROOPCNT];
SDL_Texture *potionTexture[POTION_CNT];
int troopCntW[MAX_TROOPCNT], troopCntH[MAX_TROOPCNT];

// A few map based functions

void ADD_TROOP(struct Troop t){
    map.troopList[map.troopCnt]=t;
    map.troopCnt++;
}

void DESTROY_TROOP(int ind){
    map.playerList[map.troopList[ind].owner].troopCnt--;
    map.troopList[ind]=map.troopList[map.troopCnt-1];
    map.troopCnt--;
}

void ADD_CAMPAIGN(struct Campaign c){
    map.campaignList[map.campaignCnt]=c;
    map.campaignCnt++;
}

void DESTROY_CAMPAIGN(int ind){
    map.campaignList[ind]=map.campaignList[map.campaignCnt-1];
    map.campaignCnt--;
}

void ADD_POTION(struct Potion p){
    map.potionList[map.potionCnt]=p;
    map.potionCnt++;
}

void DESTROY_POTION(int ind){
    map.potionList[ind]=map.potionList[map.potionCnt-1];
    map.potionCnt--;
}

void NEW_CAMPAIGN(int st,int fin){
    struct Campaign c;
    c.owner=map.islandList[st].owner;
    c.xStart=map.islandList[st].x+ISLAND_SIZE/2;
    c.yStart=map.islandList[st].y+ISLAND_SIZE/2;
    c.xEnd=map.islandList[fin].x+ISLAND_SIZE/2;
    c.yEnd=map.islandList[fin].y+ISLAND_SIZE/2;
    c.count=map.islandList[st].troopsCount;
    c.dest=fin;
    c.frame=(map.frameNo%FRAME_PER_OUT);
    map.islandList[st].troopsCount=0;
    ADD_CAMPAIGN(c);
}

////////////////////////////////// Game based functions

// handling clicking islands during the game
void CLICKED(int x,int y){
    if(map.selectedIsland==-1){
        for(int i=0;i<map.islandCnt;i++){
            if(COLLIDE(x,y,0,0,map.islandList[i].x+(ISLAND_SIZE-LOGO_SIZE)/2,map.islandList[i].y+(ISLAND_SIZE-LOGO_SIZE)/2,LOGO_SIZE,LOGO_SIZE)){
                if(map.islandList[i].owner==USERID){
                    map.islandList[i].isSelected=1;
                    map.selectedIsland=i;
                }
                break;
            }
        }
    }
    else{
        for(int i=0;i<map.islandCnt;i++){
            if(COLLIDE(x,y,0,0,map.islandList[i].x+(ISLAND_SIZE-LOGO_SIZE)/2,map.islandList[i].y+(ISLAND_SIZE-LOGO_SIZE)/2,LOGO_SIZE,LOGO_SIZE)){
                if(map.selectedIsland!=i){
                    NEW_CAMPAIGN(map.selectedIsland, i);
                }
                map.selectedIsland=-1;
                map.islandList[i].isSelected=0;
                break;
            }
        }
    }
}

// Sub-functions of the "MAP_UPDATE()"
void MOVE_TROOPS(int frozen){
    for(int i=0;i<map.troopCnt;i++){
        if(frozen && map.playerList[map.troopList[i].owner].potion!=FREEZE_ID) continue ;
        map.troopList[i].x+=map.troopList[i].xSpeed;
        map.troopList[i].y+=map.troopList[i].ySpeed;
        if(map.playerList[map.troopList[i].owner].potion==HASTE_ID){
            map.troopList[i].x+=map.troopList[i].xSpeed;
            map.troopList[i].y+=map.troopList[i].ySpeed;
        }
    }
}

struct Campaign HANDLE_CAMPAIGN(struct Campaign c){
    if(map.frameNo%FRAME_PER_OUT==c.frame){
        struct Troop t;
        t.owner=c.owner;
        t.x=c.xStart;
        t.y=c.yStart;
        float dx=c.xEnd-c.xStart,dy=c.yEnd-c.yStart;
        float k=sqrt((dx*dx+dy*dy)/(1.0*TROOP_SPEED*TROOP_SPEED));
        t.xSpeed=dx/k;
        t.ySpeed=dy/k;
        t.dest=c.dest;
        ADD_TROOP(t);
        c.count--;
    }
    return c;
}

void PRODUCE_TROOPS(){
    for(int i=0;i<map.islandCnt;i++){
        if(map.islandList[i].owner==0) continue ;
        if(map.frameNo%FRAME_PER_PROD==0 || (map.playerList[map.islandList[i].owner].potion==WARCRY_ID && map.frameNo%(FRAME_PER_PROD/2)==0)){
            if(map.islandList[i].troopsCount<map.islandList[i].capacity){
                map.islandList[i].troopsCount++;
                map.playerList[map.islandList[i].owner].troopCnt++;
            }
        }
    }
}

void GENERATE_POTION(){
    if(map.potionCnt<MAX_POTION && RAND(0,POTION_CHANCE)==0){
        int aInd=RAND(0, map.islandCnt), bInd=RAND(0, map.islandCnt);
        if(aInd==bInd){
            bInd=(aInd+1)%map.islandCnt;
        }
        struct Potion p;
        p.x=(map.islandList[aInd].x+map.islandList[bInd].x+ISLAND_SIZE-POTION_WIDTH)/2;
        p.y=(map.islandList[aInd].y+map.islandList[bInd].y+ISLAND_SIZE-POTION_HEIGTH)/2;
        p.type=RAND(1, 5);
        ADD_POTION(p);
    }
}

void COLLISION_CHECK(){
    // collision check (island to troop)
    for(int i=0;i<map.troopCnt;i++){
        int dest=map.troopList[i].dest;
        if(COLLIDE(map.islandList[dest].x+(ISLAND_SIZE-LOGO_SIZE)/2, map.islandList[dest].y+(ISLAND_SIZE-LOGO_SIZE)/2, LOGO_SIZE, LOGO_SIZE, map.troopList[i].x, map.troopList[i].y, TROOP_SIZE, TROOP_SIZE)){
            if(map.islandList[dest].owner==map.troopList[i].owner || map.playerList[map.islandList[dest].owner].potion==POACH_ID){
                map.islandList[dest].troopsCount++;
                map.playerList[map.islandList[dest].owner].troopCnt++;
            }
            else{
                map.islandList[dest].troopsCount--;
                map.playerList[map.islandList[dest].owner].troopCnt--;
                if(map.islandList[dest].troopsCount<0){
                    map.playerList[map.islandList[dest].owner].islandCnt--;
                    map.playerList[map.troopList[i].owner].islandCnt++;
                    map.islandList[dest].owner=map.troopList[i].owner;
                    map.islandList[dest].troopsCount=1;
                    map.playerList[map.troopList[i].owner].troopCnt++;
                }
            }
            DESTROY_TROOP(i);
        }
    }
    // collision check (potion to troop)
    for(int i=0;i<map.potionCnt;i++){
        for(int j=0;j<map.troopCnt;j++){
            if(COLLIDE(map.potionList[i].x, map.potionList[i].y, POTION_WIDTH, POTION_HEIGTH, map.troopList[j].x, map.troopList[j].y, TROOP_SIZE, TROOP_SIZE)){
                if(map.playerList[map.troopList[j].owner].potion==0){
                    map.playerList[map.troopList[j].owner].potion=map.potionList[i].type;
                    map.playerList[map.troopList[j].owner].potionLeft=POTION_LEN[map.potionList[i].type];
                    DESTROY_POTION(i);
                }
            }
        }
    }
    // collide check (troop to troop)
    for(int i=0;i<map.troopCnt;i++){
        for(int j=0;j<map.troopCnt;j++){
            if(i!=j && map.troopList[i].owner!=map.troopList[j].owner && COLLIDE(map.troopList[i].x, map.troopList[i].y, TROOP_SIZE, TROOP_SIZE, map.troopList[j].x, map.troopList[j].y, TROOP_SIZE, TROOP_SIZE)){
                DESTROY_TROOP(i);
                if(j==map.troopCnt) DESTROY_TROOP(i);
                else DESTROY_TROOP(j);
            }
        }
    }
}

void PLAYER_UPDATE(){
    for(int i=1;i<=map.playerCnt;i++){
        if(map.playerList[i].potion==0){
            continue ;
        }
        map.playerList[i].potionLeft--;
        if(map.playerList[i].potionLeft<=0){
            map.playerList[i].potion=0;
            map.playerList[i].potionLeft=0;
        }
    }
}

void AI(int id){
    for(int i=0;i<map.islandCnt;i++){
        if(map.frameNo < 2000) break;
        if(map.islandList[i].owner!=id) continue ;
        if(map.islandList[i].troopsCount>=map.islandList[i].capacity){
            int min=INF, ind=-1;
            for(int j=0;j<map.islandCnt;j++){
                if(map.islandList[j].owner==id) continue ;
                if(map.islandList[j].troopsCount<min){
                    min=map.islandList[j].troopsCount;
                    ind=j;
                }
            }
            NEW_CAMPAIGN(i, ind);
            return ;
        }
    }
    for(int i=0;i<map.islandCnt;i++){
        if(map.islandList[i].owner!=id || map.islandList[i].troopsCount<10) continue;
        int min=INF, ind=-1;
        for(int j=0;j<map.islandCnt;j++){
            if(map.islandList[j].owner==id) continue;
            if(DISTANCE(map.islandList[i].x, map.islandList[i].y, map.islandList[j].x, map.islandList[j].y)<min){
                min=DISTANCE(map.islandList[i].x, map.islandList[i].y, map.islandList[j].x, map.islandList[j].y);
                ind=j;
            }
        }
        if(map.islandList[ind].troopsCount < map.islandList[i].troopsCount){
            NEW_CAMPAIGN(i, ind);
            return ;
        }
    }
    for(int i=0;i<map.islandCnt;i++){
        if(map.frameNo < 2000) break;
        if(map.islandList[i].owner!=id || map.islandList[i].troopsCount<10) continue;
        int min=INF, ind=-1;
        for(int j=0;j<map.islandCnt;j++){
            if(map.islandList[j].owner==id || map.islandList[j].owner==USERID) continue;
            if(map.islandList[j].troopsCount > map.islandList[i].troopsCount) continue;
            if(DISTANCE(map.islandList[i].x, map.islandList[i].y, map.islandList[j].x, map.islandList[j].y)<min){
                min=DISTANCE(map.islandList[i].x, map.islandList[i].y, map.islandList[j].x, map.islandList[j].y);
                ind=j;
            }
        }
        if(ind!=-1){
            NEW_CAMPAIGN(i, ind);
            return ;
        }
    }
}

// updating the necessary parts of the map for each frame (movement, potion generation, troop production, islands state, Collisions, ...)
int MAP_UPDATE(){
    map.frameNo=(map.frameNo+1)%MAX_FRAME;

    for(int i=2;i<=map.playerCnt;i++){
        if(map.frameNo%((i+map.playerCnt)*FPS)==0) AI(i);
    }

    // figuring out if the map is frozen
    int frozen=0;
    for(int i=0;i<map.playerCnt;i++){
        if(map.playerList[i].potion==FREEZE_ID) frozen=1;
    }
    GENERATE_POTION();
    COLLISION_CHECK();
    MOVE_TROOPS(frozen);
    // handling campaigns
    for(int i=0;i<map.campaignCnt;i++){
        if(frozen && map.playerList[map.campaignList[i].owner].potion!=FREEZE_ID) continue;
        map.campaignList[i]=HANDLE_CAMPAIGN(map.campaignList[i]);
    }
    for(int i=0;i<map.campaignCnt;i++){
        while(i<map.campaignCnt && map.campaignList[i].count<=0){
            DESTROY_CAMPAIGN(i);
        }
    }
    PRODUCE_TROOPS();
    PLAYER_UPDATE();
    // Checking if the game is over
    if(map.playerList[USERID].troopCnt<=0 && map.playerList[USERID].islandCnt<=0) return -1; // user has lost
    for(int i=1;i<=map.playerCnt;i++){
        if(i!=USERID && (map.playerList[i].troopCnt>0 || map.playerList[i].islandCnt>0)){
            return 0; // game is not finished
        }
    }
    return 1; // user has won
}

void SHOW_MAP(SDL_Renderer *sdlRenderer){
    // displaying the ocean
    boxColor(sdlRenderer, 0, 0, GAME_WIDTH, GAME_HEIGTH, 0xffe19f00);
    // displaying the islands
    for(int i=0;i<map.islandCnt;i++){
        SDL_Rect islandRect = {.x=map.islandList[i].x, .y=map.islandList[i].y, .w=ISLAND_SIZE, .h=ISLAND_SIZE};
        SDL_RenderCopy(sdlRenderer, islandShape[map.islandList[i].shape][map.islandList[i].owner], NULL, &islandRect);
        SDL_Rect logoRect = {.x=map.islandList[i].x+(ISLAND_SIZE-LOGO_SIZE)/2, .y=map.islandList[i].y+(ISLAND_SIZE-LOGO_SIZE)/2, .w=LOGO_SIZE, .h=LOGO_SIZE};
        SDL_RenderCopy(sdlRenderer, logo, NULL, &logoRect);

        int num=map.islandList[i].troopsCount;
        SDL_Rect scoreRect={map.islandList[i].x+(ISLAND_SIZE-troopCntW[num])/2, map.islandList[i].y+(ISLAND_SIZE-troopCntW[num])/2, troopCntW[num], troopCntH[num]};
        SDL_RenderCopy(sdlRenderer, troopCntTexture[num], NULL, &scoreRect);
    }

    //displaying the troops
    for(int i=0;i<map.troopCnt;i++){
        filledCircleColor(sdlRenderer, map.troopList[i].x, map.troopList[i].y, TROOP_SIZE/2, 0xfff05085);
    }

    //displaying potions
    for(int i=0;i<map.potionCnt;i++){
        SDL_Rect potionRect = {.x=map.potionList[i].x, .y=map.potionList[i].y, .w=POTION_WIDTH, .h=POTION_HEIGTH};
        SDL_RenderCopy(sdlRenderer, potionTexture[map.potionList[i].type], NULL, &potionRect);
    }
}

void LOADING_SCREEN(SDL_Renderer *sdlRenderer, int *state, SDL_bool *shallExit){
    SDL_Texture *background= getImageTexture(sdlRenderer, "../Wave(2).bmp");
    SDL_Rect big={.x=0, .y=0, .h=1000, .w=1500};
    SDL_RenderCopy(sdlRenderer, background, NULL, &big);
    SDL_Texture *title= getTextTexture(sdlRenderer, "Islands.io", white, "../OpenSans-Bold.ttf", 100);
    SDL_Rect titleRect= {.x=550, .y=100, .h=150, .w=400};
    SDL_RenderCopy(sdlRenderer, title, NULL, &titleRect);
    SDL_DestroyTexture(title);
    SDL_DestroyTexture(background);

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
    SDL_Texture *title= getTextTexture(sdlRenderer, "Main Menu", black, "../OpenSans-Bold.ttf", 100);
    SDL_Rect titleRect= {.x=(SCREEN_WIDTH-300)/2, .y=200, .w=300, .h=100};
    SDL_RenderCopy(sdlRenderer, title, NULL, &titleRect);
    SDL_DestroyTexture(title);

    // displaying buttons
    const int buttonW=500, buttonH=100;
    const int buttonY[3]={ 500, 625, 750};
    for(int i=0;i<3;i++){
        roundedBoxColor(sdlRenderer, (SCREEN_WIDTH-buttonW)/2, buttonY[i], (SCREEN_WIDTH+buttonW)/2, buttonY[i]+buttonH, 10,0xff808080);
    }

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

    SDL_Texture *title= getTextTexture(sdlRenderer, "Game paused", white, "../OpenSans-Bold.ttf", 100);
    SDL_Rect titleRect= {.x=(SCREEN_WIDTH-300)/2, .y=300, .w=300, .h=100};
    SDL_RenderCopy(sdlRenderer, title, NULL, &titleRect);
    SDL_DestroyTexture(title);

    boxColor(sdlRenderer, (SCREEN_WIDTH-400)/2, 500, (SCREEN_WIDTH+400)/2, 600, 0xff827658);
    boxColor(sdlRenderer, (SCREEN_WIDTH-400)/2, 650, (SCREEN_WIDTH+400)/2, 750, 0xff293852);
    boxColor(sdlRenderer, (SCREEN_WIDTH-400)/2, 800, (SCREEN_WIDTH+400)/2, 900, 0xff938479);


    SDL_RenderPresent(sdlRenderer);
    SDL_Delay(1000/FPS);
    SDL_Event ev;
    while(SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT) {
            *shallExit = SDL_TRUE;
            break;
        }
        if(ev.type == SDL_MOUSEBUTTONDOWN){
            if(COLLIDE(ev.button.x, ev.button.y, 0, 0, (SCREEN_WIDTH-400)/2, 500, 400, 100)){
                *state=2;
            }
            if(COLLIDE(ev.button.x, ev.button.y, 0, 0, (SCREEN_WIDTH-400)/2, 650, 400, 100)){
                SAVE_GAME(map);
            }
            if(COLLIDE(ev.button.x, ev.button.y, 0, 0, (SCREEN_WIDTH-400)/2, 800, 400, 100)){
                *state=1;
            }
        }
    }
}

void NEW_GAME(SDL_Renderer *sdlRenderer, int *state, SDL_bool *shallExit, const int *mapCnt, char mapName[MAX_MAPS][50], int PlayerCntNow, int IslandCntNow){
    SDL_Texture *title= getTextTexture(sdlRenderer, "New game", black, "../OpenSans-Bold.ttf", 100);
    SDL_Rect titleRect= {.x=(SCREEN_WIDTH-300)/2, .y=200, .w=300, .h=100};
    SDL_RenderCopy(sdlRenderer, title, NULL, &titleRect);
    SDL_DestroyTexture(title);

    int buttonW=1000, buttonH=80;
    int buttonX[*mapCnt], buttonY[*mapCnt];
    for(int i=0;i<*mapCnt;i++){
        buttonX[i]=250;
        buttonY[i]=400+i*100;
        roundedBoxColor(sdlRenderer, buttonX[i], buttonY[i], buttonX[i]+buttonW, buttonY[i]+buttonH, 10, 0xff808080);
    }
    roundedBoxColor(sdlRenderer, 0, 0, 100, 100, 5, 0xffff8000);

    roundedBoxColor(sdlRenderer, 1300, 850, 1400, 950, 10, 0xff987345);

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
            if(COLLIDE(ev.button.x, ev.button.y, 1, 1, 0, 0, 100, 100)){
                *state=5;
                map= MAP_GENERATOR(IslandCntNow, PlayerCntNow);
            }
            if( COLLIDE(ev.button.x, ev.button.y, 1, 1, 1300, 850, 100, 100)){
                *state=1;
            }
        }
    }
}

void CONTINUE(SDL_Renderer *sdlRenderer, int *state, SDL_bool *shallExit){
    SDL_Texture *title= getTextTexture(sdlRenderer, "Continue", black, "../OpenSans-Bold.ttf", 100);
    SDL_Rect titleRect= {.x=(SCREEN_WIDTH-300)/2, .y=200, .w=300, .h=100};
    SDL_RenderCopy(sdlRenderer, title, NULL, &titleRect);
    SDL_DestroyTexture(title);

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
    SDL_Texture *title= getTextTexture(sdlRenderer, "Scoreboard", black, "../OpenSans-Bold.ttf", 100);
    SDL_Rect titleRect= {.x=(SCREEN_WIDTH-300)/2, .y=200, .w=300, .h=100};
    SDL_RenderCopy(sdlRenderer, title, NULL, &titleRect);
    SDL_DestroyTexture(title);
    struct Scoreboard sb=LOAD_SCOREBOARD();
    for(int i=0;i<sb.userCnt;i++){
        TTF_Font *font= TTF_OpenFont("../OpenSans-Regular.ttf", 40);
        int w=0,h=0;
        TTF_SizeText(font, sb.nameList[i], &w, &h);
        SDL_Rect textRect={400, 300+i*100, w, h};
        SDL_Texture *text= getTextTexture(sdlRenderer, sb.nameList[i], black, "../OpenSans-Regular.ttf", 40);
        SDL_RenderCopy(sdlRenderer, text, NULL, &textRect);
        SDL_DestroyTexture(text);
        TTF_SizeText(font, TO_STRING(sb.scoreList[i]), &w, &h);
        SDL_Rect scoreRect={1000, 300+i*100, w, h};
        SDL_Texture *score= getTextTexture(sdlRenderer, TO_STRING(sb.scoreList[i]), black, "../OpenSans-Regular.ttf", 40);
        SDL_RenderCopy(sdlRenderer, score, NULL, &scoreRect);
        SDL_DestroyTexture(score);
    }

    roundedBoxColor(sdlRenderer, 1300, 850, 1400, 950, 10, 0xff287346);

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

void LOAD_TEXTURES(SDL_Renderer *sdlRenderer){
    regularFont= TTF_OpenFont("../OpenSans-Regular.ttf", 20);

    // Island designs
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
    logo = getImageTexture(sdlRenderer, "../star.bmp");

    // number textures
    regularFont= TTF_OpenFont("../OpenSans-Regular.ttf", 15);
    for(int i=0;i<MAX_TROOPCNT;i++){
        troopCntTexture[i]= getTextTexture(sdlRenderer, TO_STRING(i), white, "../OpenSans-Regular.ttf", 15);
        TTF_SizeText(regularFont, TO_STRING(i), &troopCntW[i], &troopCntH[i]);
    }
    TTF_CloseFont(regularFont);

    // potions
    potionTexture[FREEZE_ID]= getImageTexture(sdlRenderer, "../Potions/FreezePotion.bmp");
    potionTexture[HASTE_ID]= getImageTexture(sdlRenderer, "../Potions/HastePotion.bmp");
    potionTexture[POACH_ID]= getImageTexture(sdlRenderer, "../Potions/PoachPotion.bmp");
    potionTexture[WARCRY_ID]= getImageTexture(sdlRenderer, "../Potions/WarcryPotion.bmp");
}

int main() {
    srand(time(NULL));
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }
    if(TTF_Init()<0){
        printf("Error Loading TTF\n");
    }
    // Source: https://stackoverflow.com/questions/1121383/counting-the-number-of-files-in-a-directory-using-c (for counting the files in directory)
    int mapCnt = 0;
    char mapName[MAX_MAPS][50];
    DIR * dirp;
    struct dirent * entry;
    dirp = opendir("../Maps");
    while ((entry = readdir(dirp)) != NULL) {
        if (entry->d_type == DT_REG) {
            memset(mapName[mapCnt], 0, 50);
            strcpy(mapName[mapCnt], entry->d_name);
            mapCnt++;
        }
    }
    closedir(dirp);

    ///////////// main
    SDL_Window *sdlWindow = SDL_CreateWindow("Test_window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH,SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
    SDL_Renderer *sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

    LOAD_TEXTURES(sdlRenderer);

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

    char userName[50], mapID[50];
    int IslandCntNow=15, PlayerCntNow=3;
    memset(userName, 0, 50);
    memset(mapID, 0, 50);
    SDL_bool shallExit = SDL_FALSE;
    // Game Loop
    while(shallExit == SDL_FALSE){
        SDL_SetRenderDrawColor(sdlRenderer, 0xff, 0xff, 0xff, 0xff); //#0fa8ce
        SDL_RenderClear(sdlRenderer);

        if(state==0){
            LOADING_SCREEN(sdlRenderer, &state, &shallExit);
        }
        else if(state==1){
            MENU(sdlRenderer, &state, &shallExit);
        }
        else if(state==2){
            // Updating everything
            int isOver=MAP_UPDATE();

            SHOW_MAP(sdlRenderer);

            if(isOver==-1){ // User lost
                boxColor(sdlRenderer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0x80000000);
                SDL_Texture *text= getTextTexture(sdlRenderer, "You LOST!", white, "../OpenSans-Bold.ttf", 100);
                SDL_Rect textRect={.x=SCREEN_WIDTH/2-100, .y=400, .w=200, .h=100};
                SDL_RenderCopy(sdlRenderer, text, NULL, &textRect);
                struct Scoreboard sb=LOAD_SCOREBOARD();
                ADD_TO_SCOREBOARD(&sb, userName, -LOSE_SCORE);
                SAVE_SCOREBOARD(sb);
                state=1;
            }
            if(isOver==1){ // User won
                boxColor(sdlRenderer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0x80000000);
                SDL_Texture *text= getTextTexture(sdlRenderer, "You WON!", white, "../OpenSans-Bold.ttf", 100);
                SDL_Rect textRect={.x=SCREEN_WIDTH/2-100, .y=400, .w=200, .h=100};
                SDL_RenderCopy(sdlRenderer, text, NULL, &textRect);
                struct Scoreboard sb=LOAD_SCOREBOARD();
                ADD_TO_SCOREBOARD(&sb, userName, WIN_SCORE);
                SAVE_SCOREBOARD(sb);
                state=1;
            }

            roundedBoxColor(sdlRenderer, 1300, 850, 1400, 950, 10, 0xff928347);

            SDL_RenderPresent(sdlRenderer);
            SDL_Delay(1000 / FPS);
            SDL_Event ev;
            while(SDL_PollEvent(&ev)){
                if(ev.type==SDL_QUIT){
                    shallExit=SDL_TRUE;
                    break;
                }
                if(ev.type==SDL_MOUSEBUTTONDOWN){
                    CLICKED(ev.button.x,ev.button.y);
                    if( COLLIDE(ev.button.x, ev.button.y, 1, 1, 1300, 850, 100, 100)){
                        state=3;
                    }
                }
            }

            if(isOver!=0) SDL_Delay(3000);
        }
        else if(state==3){
            GAME_PAUSED(sdlRenderer, &state, &shallExit);
        }
        else if(state==4){
            NEW_GAME(sdlRenderer, &state, &shallExit, &mapCnt, mapName, PlayerCntNow, IslandCntNow);
        }
        else if(state==5){
            SDL_Texture *title= getTextTexture(sdlRenderer, "New map", black, "../OpenSans-Bold.ttf", 100);
            SDL_Rect titleRect= {.x=(SCREEN_WIDTH-300)/2, .y=200, .w=300, .h=100};
            SDL_RenderCopy(sdlRenderer, title, NULL, &titleRect);
            SDL_DestroyTexture(title);

            SHOW_MAP(sdlRenderer);

            TTF_Font *font= TTF_OpenFont("../OpenSans-Regular.ttf", 50);
            int w,h;
            TTF_SizeText(font, mapID, &w, &h);
            SDL_Rect textRect={(SCREEN_WIDTH-w)/2, 900-h/2, w, h};
            boxColor(sdlRenderer, (SCREEN_WIDTH-w)/2, 900-h/2, (SCREEN_WIDTH+w)/2, 900+h/2, 0x80431562);
            SDL_Texture *text= getTextTexture(sdlRenderer, mapID, black, "../OpenSans-Regular.ttf", 50);
            SDL_RenderCopy(sdlRenderer, text, NULL, &textRect);
            SDL_DestroyTexture(text);

            roundedBoxColor(sdlRenderer, 1300, 850, 1400, 950, 10, 0xff137722);

            SDL_RenderPresent(sdlRenderer);
            SDL_Delay(1000/FPS);
            SDL_Event ev;
            while(SDL_PollEvent(&ev)) {
                if (ev.type == SDL_QUIT) {
                    shallExit = SDL_TRUE;
                    break;
                }
                else if(ev.type == SDL_MOUSEBUTTONDOWN){
                    if( COLLIDE(ev.button.x, ev.button.y, 1, 1, 1300, 850, 100, 100)){
                        state=4;
                    }
                    map= MAP_GENERATOR(IslandCntNow, PlayerCntNow);
                }
                else if(ev.type==SDL_TEXTINPUT){
                    strcat(mapID, ev.text.text);
                }
                else if(ev.type==SDL_KEYDOWN && ev.key.keysym.sym==SDLK_BACKSPACE){
                    mapID[strlen(mapID)-1]=0;
                }
                else if(ev.type==SDL_KEYDOWN && ev.key.keysym.sym==SDLK_RETURN){
                    state=4;
                    SAVE_MAP(map, mapID);
                    memset(mapName[mapCnt], 0, 50);
                    strcpy(mapName[mapCnt], mapID);
                    mapCnt++;
                    memset(mapID, 0, 50);
                }
            }
        }
        else if(state==6){
            CONTINUE(sdlRenderer, &state, &shallExit);
        }
        else if(state==7){
            SCOREBOARD(sdlRenderer, &state, &shallExit);
        }
        else if(state==8){
            SDL_Texture *title= getTextTexture(sdlRenderer, "Enter Name", black, "../OpenSans-Bold.ttf", 100);
            SDL_Rect titleRect= {.x=(SCREEN_WIDTH-300)/2, .y=200, .w=300, .h=100};
            SDL_RenderCopy(sdlRenderer, title, NULL, &titleRect);
            SDL_DestroyTexture(title);

            TTF_Font *font= TTF_OpenFont("../OpenSans-Regular.ttf", 50);
            int w,h;
            TTF_SizeText(font, userName, &w, &h);
            SDL_Rect textRect={(SCREEN_WIDTH-w)/2, (SCREEN_HEIGHT-h)/2, w, h};
            boxColor(sdlRenderer, (SCREEN_WIDTH-w)/2, (SCREEN_HEIGHT-h)/2, (SCREEN_WIDTH+w)/2, (SCREEN_HEIGHT+h)/2, 0x8098a558);
            SDL_Texture *text= getTextTexture(sdlRenderer, userName, black, "../OpenSans-Regular.ttf", 50);
            SDL_RenderCopy(sdlRenderer, text, NULL, &textRect);
            SDL_DestroyTexture(text);

            SDL_RenderPresent(sdlRenderer);
            SDL_Delay(1000/FPS);
            SDL_StartTextInput();
            SDL_Event ev;
            while(SDL_PollEvent(&ev)) {
                if (ev.type == SDL_QUIT) {
                    shallExit = SDL_TRUE;
                    break;
                }
                else if(ev.type==SDL_TEXTINPUT){
                    strcat(userName, ev.text.text);
                }
                else if(ev.type==SDL_KEYDOWN && ev.key.keysym.sym==SDLK_BACKSPACE){
                    userName[strlen(userName)-1]=0;
                }
                else if(ev.type==SDL_KEYDOWN && ev.key.keysym.sym==SDLK_RETURN){
                    state=1;
                }
            }
        }
    }
    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();
    TTF_Quit();
    return 0;
}
