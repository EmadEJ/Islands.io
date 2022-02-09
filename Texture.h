//
// Created by emad on 02/02/2022.
//

#ifndef TEST_GRAPHICS_TEXTURE_H
#define TEST_GRAPHICS_TEXTURE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>
#include "Perimeters.h"

// for images
SDL_Texture *getImageTexture(SDL_Renderer *sdlRenderer, char *image_path) {
    SDL_Surface *image = SDL_LoadBMP(image_path);
    /* Let the user know if the file failed to load */
    if (!image) {
        printf("Failed to load image at %s: %s\n", image_path, SDL_GetError());
        return 0;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(sdlRenderer, image);
    SDL_FreeSurface(image);
    image=NULL;
    return texture;
}

// for text
SDL_Texture *getTextTexture(SDL_Renderer *sdlRenderer, char *text, SDL_Color color, char *fontPath, int size){
    TTF_Font *font= TTF_OpenFont(fontPath, size);
    SDL_Surface *tmp= TTF_RenderText_Blended(font, text, color);
    SDL_Texture *texture= SDL_CreateTextureFromSurface(sdlRenderer, tmp);
    SDL_FreeSurface(tmp);
    TTF_CloseFont(font);
    tmp=NULL;
    return texture;
}

void putImage(SDL_Renderer *sdlRenderer, char *image_path, int x, int y, int w,int h) {
    SDL_Rect rect= {x, y, w, h};
    SDL_Texture *texture= getImageTexture(sdlRenderer, image_path);
    SDL_RenderCopy(sdlRenderer, texture, NULL, &rect);
    SDL_DestroyTexture(texture);
}

void putText(SDL_Renderer *sdlRenderer, char *text, SDL_Color color, char *fontPath, int size, int x, int y){
    TTF_Font *font= TTF_OpenFont(fontPath, size);
    int w,h;
    TTF_SizeText(font, text, &w, &h);
    SDL_Rect textRect={x, y, w, h};
    SDL_Texture *textText= getTextTexture(sdlRenderer, text, color, fontPath, size);
    SDL_RenderCopy(sdlRenderer, textText, NULL, &textRect);
    SDL_DestroyTexture(textText);
    TTF_CloseFont(font);
}

void putTextMid(SDL_Renderer *sdlRenderer, char *text, SDL_Color color, char *fontPath, int size, int y){
    TTF_Font *font= TTF_OpenFont(fontPath, size);
    int w,h;
    TTF_SizeText(font, text, &w, &h);
    int x=SCREEN_WIDTH/2-w/2;
    SDL_Rect textRect={x, y, w, h};
    SDL_Texture *textText= getTextTexture(sdlRenderer, text, color, fontPath, size);
    SDL_RenderCopy(sdlRenderer, textText, NULL, &textRect);
    SDL_DestroyTexture(textText);
    TTF_CloseFont(font);
}

#endif //TEST_GRAPHICS_TEXTURE_H
