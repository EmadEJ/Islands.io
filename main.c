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
#include <dirent.h>

#include "Perimeters.h"
#include "Map.h"
#include "Functions.h"

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
}

struct Map LOAD_GAME(){
    const char *path= "../lastGame.dat";
    FILE *fp;
    fp= fopen(path, "w");
    struct Map res;
    fwrite(&res, sizeof(struct Map), 1, fp);
    return res;
}

// A few map based functions

void ADD_TROOP(struct Troop t){
    map.troopList[map.troopCnt]=t;
    map.troopCnt++;
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

void ADD_POTION(struct Potion p){
    map.potionList[map.potionCnt]=p;
    map.potionCnt++;
}

void DESTROY_POTION(int ind){
    map.potionList[ind]=map.potionList[map.potionCnt-1];
    map.potionCnt--;
}

////////////////////////////////// Game based functions

// handling clicking islands during the game
void CLICKED(int x,int y,int frameNo){
    if(map.selectedIsland==-1){
        for(int i=0;i<map.islandCnt;i++){
            if(COLLIDE(x,y,0,0,map.islandList[i].x+(ISLAND_SIZE-LOGO_SIZE)/2,map.islandList[i].y+(ISLAND_SIZE-LOGO_SIZE)/2,LOGO_SIZE,LOGO_SIZE)){
    //            if(map.islandList[i].owner==USERID){
                    map.islandList[i].isSelected=1;
                    map.selectedIsland=i;
                    printf("Island Selected\n");
    //            }
                break;
            }
        }
    }
    else{
        for(int i=0;i<map.islandCnt;i++){
            if(COLLIDE(x,y,0,0,map.islandList[i].x+(ISLAND_SIZE-LOGO_SIZE)/2,map.islandList[i].y+(ISLAND_SIZE-LOGO_SIZE)/2,LOGO_SIZE,LOGO_SIZE)){
                if(map.selectedIsland!=i){
                    struct Campaign c;
                    c.owner=map.islandList[map.selectedIsland].owner;
                    c.xStart=map.islandList[map.selectedIsland].x+ISLAND_SIZE/2;
                    c.yStart=map.islandList[map.selectedIsland].y+ISLAND_SIZE/2;
                    c.xEnd=map.islandList[i].x+ISLAND_SIZE/2;
                    c.yEnd=map.islandList[i].y+ISLAND_SIZE/2;
                    c.count=map.islandList[map.selectedIsland].troopsCount;
                    c.dest=i;
                    c.frame=(frameNo%FRAME_PER_OUT);
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
    if(frameNo%FRAME_PER_OUT==c.frame){
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
    // potion generation (by putting a random potion between random islands a and b)
    if(map.potionCnt<MAX_POTION && RAND(0,POTION_CHANCE)==0){
        printf("!\n");
        int aInd=RAND(0, map.islandCnt), bInd=RAND(0, map.islandCnt);
        if(aInd==bInd){
            bInd=(aInd+1)%map.islandCnt;
        }
        struct Potion p;
        p.x=(map.islandList[aInd].x+map.islandList[bInd].x+ISLAND_SIZE-POTION_SIZE)/2;
        p.y=(map.islandList[aInd].y+map.islandList[bInd].y+ISLAND_SIZE-POTION_SIZE)/2;
        p.type=RAND(1, 5);
        ADD_POTION(p);
    }
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
    for(int i=0;i<map.potionCnt;i++){
        for(int j=0;j<map.troopCnt;j++){
            if(COLLIDE(map.potionList[i].x, map.potionList[i].y, POTION_SIZE, POTION_SIZE, map.troopList[j].x, map.troopList[j].y, TROOP_SIZE, TROOP_SIZE)){
                if(map.playerList[map.troopList[j].owner].potion==0){
                    map.playerList[map.troopList[j].owner].potion=map.potionList[i].type;
                    map.playerList[map.troopList[j].owner].potionLeft=POTION_LEN;
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
    // Updating players (potions, islandCnt, ...)

}

int main() {
    srand(time(NULL));
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    // Source: https://stackoverflow.com/questions/1121383/counting-the-number-of-files-in-a-directory-using-c
    int mapCnt = 0;
    DIR * dirp;
    struct dirent * entry;
    dirp = opendir("../Maps");
    while ((entry = readdir(dirp)) != NULL) {
        if (entry->d_type == DT_REG) {
            mapCnt++;
        }
    }
    closedir(dirp);
    printf("%d\n", mapCnt);

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
            islandShape[i][j]= getImageTexture(sdlRenderer, str);
        }
    }
    SDL_Texture *logo = getImageTexture(sdlRenderer, "../star.bmp");

    map= MAP_GENERATOR(15, 4);

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

        //displaying potions
        for(int i=0;i<map.potionCnt;i++){
            SDL_Rect potionRect = {.x=map.potionList[i].x, .y=map.potionList[i].y, .w=POTION_SIZE, .h=POTION_SIZE};
            SDL_RenderCopy(sdlRenderer, logo, NULL, &potionRect);
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
                CLICKED(sdlEvent.button.x,sdlEvent.button.y, frameNo);
            }
        }
    }
    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();
    return 0;
}
