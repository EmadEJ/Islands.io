//
// Created by Emad on 23/01/2022.
//

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include "Map.h"
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

#define USERID 1

// default perimeters
#define FRAME_PER_PROD 60
#define FRAME_PER_OUT 20
#define TROOP_SPEED 3

// Potion IDs
#define FREEZE_ID 1
#define HASTE_ID 2
#define DECEPTION_ID 3
#define WARCRY_ID 4

// Screen info
#define SCREEN_WIDTH 1500
#define SCREEN_HEIGHT 1000
#define FPS 60

// for images (not made by me)
SDL_Texture *getImageTexture(SDL_Renderer *sdlRenderer, char *image_path) {
    SDL_Surface *image = SDL_LoadBMP(image_path);

    /* Let the user know if the file failed to load */
    if (!image) {
        printf("Failed to load image at %s: %s\n", image_path, SDL_GetError());
        return 0;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(sdlRenderer, image);

    SDL_FreeSurface(image);
    image = NULL;

    return texture;
}

struct Map map;

// A few map based functions

void ADD_TROOP(struct Troop t){
    int ind=map.troopCnt;
    map.troopCnt++;
    map.troopList[ind]=t;
}

void DESTROY_TROOP(int ind){
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

////////////////////////////////// Game based functions

// handling clicking islands during the game
void CLICKED(int x,int y){
    if(map.selectedIsland==-1){
        for(int i=0;i<map.islandCnt;i++){
            if(COLLIDE(x,y,0,0,map.islandList[i].x+(ISLAND_SIZE-LOGO_SIZE)/2,map.islandList[i].y+(ISLAND_SIZE-LOGO_SIZE)/2,LOGO_SIZE,LOGO_SIZE)){
                if(map.islandList[i].owner==USERID){
                    map.islandList[i].isSelected=1;
                    map.selectedIsland=i;
                    printf("Island Selected\n");
                }
                break;
            }
        }
    }
    else{
        for(int i=0;i<map.islandCnt;i++){
            if(COLLIDE(x,y,0,0,map.islandList[i].x+(ISLAND_SIZE-LOGO_SIZE)/2,map.islandList[i].y+(ISLAND_SIZE-LOGO_SIZE)/2,LOGO_SIZE,LOGO_SIZE)){
                if(map.selectedIsland!=i){
                    struct Campaign c;
                    c.owner=USERID;
                    c.xStart=map.islandList[map.selectedIsland].x+ISLAND_SIZE/2;
                    c.yStart=map.islandList[map.selectedIsland].y+ISLAND_SIZE/2;
                    c.xEnd=map.islandList[i].x+ISLAND_SIZE/2;
                    c.yEnd=map.islandList[i].y+ISLAND_SIZE/2;
                    c.count=map.islandList[map.selectedIsland].troopsCount;
                    c.dest=i;
                    map.islandList[map.selectedIsland].troopsCount=0;
                    ADD_CAMPAIGN(c);
                    printf("Campaign Added from %d to %d\n", map.selectedIsland, i);
                }
                map.selectedIsland=-1;
                map.islandList[i].isSelected=0;
                break;
            }
        }
    }
}

void MOVE_TROOPS(){
    for(int i=0;i<map.troopCnt;i++){
        map.troopList[i].x+=map.troopList[i].xSpeed;
        map.troopList[i].y+=map.troopList[i].ySpeed;
    }
}

struct Campaign HANDLE_CAMPAIGN(struct Campaign c, int frameNo){
    if(frameNo%FRAME_PER_OUT==0){
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

void PRODUCE_TROOPS(int frameNo){
    for(int i=0;i<map.islandCnt;i++){
        if(map.islandList[i].owner==0) continue ;
        if(frameNo%FRAME_PER_PROD==0 || (map.playerList[map.islandList[i].owner].potion==WARCRY_ID && frameNo%FRAME_PER_PROD==0)){
            if(map.islandList[i].troopsCount<map.islandList[i].capacity){
                map.islandList[i].troopsCount++;
            }
        }
    }
}

// updating the necessary parts of the map for each frame (movement, potion generation, troop production, islands state)
void MAP_UPDATE(int frameNo){
    // potion generation

    // collide check (island to troop)
    for(int i=0;i<map.troopCnt;i++){
        int dest=map.troopList[i].dest;
        if(COLLIDE(map.islandList[dest].x+(ISLAND_SIZE-LOGO_SIZE)/2, map.islandList[dest].y+(ISLAND_SIZE-LOGO_SIZE)/2, LOGO_SIZE, LOGO_SIZE, map.troopList[i].x, map.troopList[i].y, TROOP_SIZE, TROOP_SIZE)){
            if(map.islandList[dest].owner==map.troopList[i].owner){
                map.islandList[dest].troopsCount++;
            }
            else{
                map.islandList[dest].troopsCount--;
                if(map.islandList[dest].troopsCount<0){
                    map.islandList[dest].owner=map.troopList[i].owner;
                    map.islandList[dest].troopsCount=1;
                }
            }
            DESTROY_TROOP(i);
        }
    }
    // collide check (potion to troop)

    // collide check (troop to troop)

    // moving troops
    MOVE_TROOPS();
    // handling campaigns
    for(int i=0;i<map.campaignCnt;i++){
        map.campaignList[i]=HANDLE_CAMPAIGN(map.campaignList[i], frameNo);
    }
    for(int i=0;i<map.campaignCnt;i++){
        while(i<map.campaignCnt && map.campaignList[i].count<=0){
            DESTROY_CAMPAIGN(i);
        }
    }
    // producing troops
    PRODUCE_TROOPS(frameNo);
}

int main() {
    srand(time(NULL));
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }
    ///////////// main
    SDL_Window *sdlWindow = SDL_CreateWindow("Test_window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH,SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
    SDL_Renderer *sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

    // texture loading
    SDL_Texture *islandShape[MAX_SHAPE][MAX_PLAYER+1];
    for(int i=0;i<MAX_SHAPE;i++){
        for(int j=0;j<=MAX_PLAYER;j++){
            char str[40];
            memset(str, 0, 40);
            strcpy(str, "../Islands/Island");
            str[strlen(str)]='0'+(i/10);
            str[strlen(str)]='0'+(i%10);
            str[strlen(str)]='0'+j;
            strcat(str, ".bmp");
            printf("%s\n", str);
            islandShape[i][j]= getImageTexture(sdlRenderer, str);
        }
    }
    SDL_Texture *logo = getImageTexture(sdlRenderer, "../star.bmp");
    SDL_Texture *island = getImageTexture(sdlRenderer, "../island.bmp");

    map= MAP_GENERATOR(10,3);

    // game Loop
    SDL_bool shallExit = SDL_FALSE;
    for(int frameNo=0;shallExit == SDL_FALSE;frameNo=(frameNo+1)%MAX_FRAME){
        SDL_SetRenderDrawColor(sdlRenderer, 0x0f, 0xa8, 0xce, 0xff); //#0fa8ce
        SDL_RenderClear(sdlRenderer);

        // Updating everything
        MAP_UPDATE(frameNo);

        // map log


        // displaying the islands
        for(int i=0;i<map.islandCnt;i++){
            SDL_Rect islandRect = {.x=map.islandList[i].x, .y=map.islandList[i].y, .w=ISLAND_SIZE, .h=ISLAND_SIZE};
            SDL_RenderCopy(sdlRenderer, islandShape[map.islandList[i].shape][map.islandList[i].owner], NULL, &islandRect);
            SDL_Rect logoRect = {.x=map.islandList[i].x+(ISLAND_SIZE-LOGO_SIZE)/2, .y=map.islandList[i].y+(ISLAND_SIZE-LOGO_SIZE)/2, .w=LOGO_SIZE, .h=LOGO_SIZE};
            SDL_RenderCopy(sdlRenderer, logo, NULL, &logoRect);
        }

        //displaying the troops
        for(int i=0;i<map.troopCnt;i++){
            filledCircleColor(sdlRenderer, map.troopList[i].x, map.troopList[i].y, TROOP_SIZE/2, 0xfff05085);
        }

        SDL_RenderPresent(sdlRenderer);
        SDL_Delay(1000 / FPS);
        SDL_Event sdlEvent;
        while(SDL_PollEvent(&sdlEvent)){
            if(sdlEvent.type==SDL_QUIT){
                shallExit=SDL_TRUE;
                break;
            }
            if(sdlEvent.type==SDL_MOUSEBUTTONDOWN){
                CLICKED(sdlEvent.button.x,sdlEvent.button.y);
            }
        }
    }
    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();
    return 0;
}
