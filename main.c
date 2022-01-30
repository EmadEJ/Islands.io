#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_ISLANDS 40
#define ISLAND_SIZE 150
#define LOGO_SIZE 40

const int SCREEN_WIDTH = 1500,SCREEN_HEIGHT = 1000;
const int FPS=60;

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

/// Useful general functions
int RAND(int max){
    return (rand()%max);
}

int DISTANCE(int x1,int y1,int x2,int y2){
    return (x1-x2)*(x1-x2)+(y1-y2)*(y1-y2);
}

struct Island{
    int troopsCount,capacity;
    int owner;
    int shapeNo;
    int x,y;
};

struct Map{
    int islandCnt;
    struct Island islandList[MAX_ISLANDS];
    int playerCnt;
};

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
            x=RAND(SCREEN_WIDTH-ISLAND_SIZE);
            y=RAND(SCREEN_HEIGHT-ISLAND_SIZE);
            for(int j=0;j<i;j++) {
                if (ISLAND_COLLIDE(x, y, res.islandList[j].x, res.islandList[j].y)) {
                    isok = 0;
                    break;
                }
            }
        }
        res.islandList[i].x=x;
        res.islandList[i].y=y;
    }
    return res;
};

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

    struct Map map= MAP_GENERATOR(20,3);

    for(int i=0;i<map.islandCnt;i++){
        printf("%d %d\n", map.islandList[i].x, map.islandList[i].y);
    }

    SDL_bool shallExit = SDL_FALSE;
    while(shallExit == SDL_FALSE) {
        SDL_SetRenderDrawColor(sdlRenderer, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderClear(sdlRenderer);

        for(int i=0;i<map.islandCnt;i++){
            SDL_Rect islandRect = {.x=map.islandList[i].x, .y=map.islandList[i].y, .w=ISLAND_SIZE, .h=ISLAND_SIZE};
            SDL_RenderCopy(sdlRenderer, island, NULL, &islandRect);
            SDL_Rect logoRect = {.x=map.islandList[i].x+55, .y=map.islandList[i].y+55, .w=LOGO_SIZE, .h=LOGO_SIZE};
            SDL_RenderCopy(sdlRenderer, logo, NULL, &logoRect);
        }

        filledCircleColor(sdlRenderer, SCREEN_WIDTH , SCREEN_HEIGHT , 30, 0xff123263);

        SDL_RenderPresent(sdlRenderer);
        SDL_Delay(1000 / FPS);
        SDL_Event sdlEvent;
        while(SDL_PollEvent(&sdlEvent)){
            if(sdlEvent.type==SDL_QUIT){
                shallExit=SDL_TRUE;
                break;
            }
        }
    }

    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();
    return 0;
}
