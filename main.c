#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_ISLANDS 25
#define ISLAND_SIZE 150
#define LOGO_SIZE 60
#define TROOP_SIZE 20
#define MAX_TROOPS 1000
#define MAX_CAMPAIGN
#define USERID 1

const int SCREEN_WIDTH = 1500,SCREEN_HEIGHT = 1000;
const int FPS=60;

//////////// global variables

////////////

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

/////////////////////// Useful general functions
// generates random between mn and mx
int RAND(int mn, int mx){
    return mn+(rand()%(mx-mn));
}

// returns the distance between 2 points (^2)
int DISTANCE(int x1,int y1,int x2,int y2){
    return (x1-x2)*(x1-x2)+(y1-y2)*(y1-y2);
}

// checks if two boxes collide
int COLLIDE(int x1,int y1,int w1,int h1,int x2,int y2,int w2,int h2){
    if(x1>x2+w2 || x1+w1<x2) return 0;
    if(y1>y2+h2 || y1+h1<y2) return 0;
    return 1;
}

// holds the data for an island
struct Island{
    int troopsCount,capacity;
    int owner;
    int shapeNo;
    int isSelected;
    int x,y;
};

// holds the data for a troop
struct Troop{
    int x,y;
    int xSpeed,ySpeed;
    int owner;
};

// holds the data for a campaign (*Lashkar-Keshi)
struct Campaign{
    int xStart,yStart;
    int xEnd,yEnd;
    int owner;
};

// holds the data about the map (the whole game actually)
struct Map{
    int islandCnt;
    struct Island islandList[MAX_ISLANDS];
    int selectedIsland;
    int playerCnt;
    int troopCnt;
    struct Troop troopList[MAX_TROOPS];
    int campaignCnt;
    struct Campaign campaignList[MAX_CAMPAIGN];
};
struct Map map;

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

int ISLAND_COLLIDE(int x1, int y1, int x2,int y2){
    int dis=(ISLAND_SIZE+LOGO_SIZE)/2;
    if( (x1<x2+dis && x1>x2-dis) && (y1<y2+dis && y1>y2-dis) ) return 1;
    return 0;
}

struct Map MAP_GENERATOR(int islandCnt, int enemyCnt){
    struct Map res;
    res.islandCnt=islandCnt;
    for(int i=0;i<islandCnt;i++){
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
        res.islandList[i].x=x;
        res.islandList[i].y=y;
        res.islandList[i].isSelected=0;
    }
    res.troopCnt=0;
    res.campaignCnt=0;
    map.selectedIsland=-1;
    return res;
};

void TROOP_COLLIDE(){

}

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
                    struct Campaign c;
                    c.owner=USERID;
                    c.xStart=map.islandList[map.selectedIsland].x;
                    c.yStart=map.islandList[map.selectedIsland].y;
                    c.xEnd=map.islandList[i].x;
                    c.yEnd=map.islandList[i].y;
                    ADD_CAMPAIGN(c);
                }
                map.selectedIsland=-1;
                map.islandList[i].isSelected=0;
                break;
            }
        }
    }
}

void MAP_UPDATE(){
    for(int i=0;i<map.troopCnt;i++){

    }
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

    SDL_Texture *logo = getImageTexture(sdlRenderer, "../star.bmp");
    SDL_Texture *island = getImageTexture(sdlRenderer, "../island.bmp");

    map= MAP_GENERATOR(20,3);

    for(int i=0;i<map.islandCnt;i++){
        printf("%d %d\n", map.islandList[i].x, map.islandList[i].y);
    }

    Sint16 xx=300,yy=300;

    SDL_bool shallExit = SDL_FALSE;
    while(shallExit == SDL_FALSE) {
        SDL_SetRenderDrawColor(sdlRenderer, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderClear(sdlRenderer);

        // displaying the islands
        for(int i=0;i<map.islandCnt;i++){
            SDL_Rect islandRect = {.x=map.islandList[i].x, .y=map.islandList[i].y, .w=ISLAND_SIZE, .h=ISLAND_SIZE};
            SDL_RenderCopy(sdlRenderer, island, NULL, &islandRect);
            SDL_Rect logoRect = {.x=map.islandList[i].x+(ISLAND_SIZE-LOGO_SIZE)/2, .y=map.islandList[i].y+(ISLAND_SIZE-LOGO_SIZE)/2, .w=LOGO_SIZE, .h=LOGO_SIZE};
            SDL_RenderCopy(sdlRenderer, logo, NULL, &logoRect);
        }

        //displaying the troops
        xx+=2;
        yy+=0;
        filledCircleColor(sdlRenderer, xx, yy, 25, 0xeb3324ff);

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
