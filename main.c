#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define MAX_ISLANDS 40
#define ISLAND_SIZE 100
#define LOGO_SIZE 40

const int SCREEN_WIDTH = 1440,SCREEN_HEIGHT = 1080;
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

int RAND(int max){
    return (rand()%max);
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

struct Map MAP_GENERATOR(int islandCnt, int enemyCnt){
    struct Map res;
    for(int i=0;i<islandCnt;i++){
        res.islandList[i].x=RAND(SCREEN_WIDTH-ISLAND_SIZE);
        res.islandList[i].y=RAND(SCREEN_HEIGHT-ISLAND_SIZE);
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

    struct Map map= MAP_GENERATOR(30,3);

    for(int i=0;i<map.islandCnt;i++){
        printf("%d %d\n", map.islandList[i].x, map.islandList[i].y);
    }

    SDL_bool shallExit = SDL_FALSE;
    while(shallExit == SDL_FALSE) {
        SDL_SetRenderDrawColor(sdlRenderer, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderClear(sdlRenderer);

        for(int i=0;i<map.islandCnt;i++){
            SDL_Rect rect = {.x=map.islandList[i].x, .y=map.islandList[i].y, .w=ISLAND_SIZE, .h=ISLAND_SIZE};
            SDL_RenderCopy(sdlRenderer, island, NULL, &rect);
        }

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
