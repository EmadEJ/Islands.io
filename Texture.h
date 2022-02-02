//
// Created by emad on 02/02/2022.
//

#ifndef TEST_GRAPHICS_TEXTURE_H
#define TEST_GRAPHICS_TEXTURE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>

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
    image = NULL;

    return texture;
}

// for text
SDL_Texture *getTextTexture(SDL_Renderer *sdlRenderer, char *text, SDL_Color color, char *fontPath){
    TTF_Font *font= TTF_OpenFont(fontPath, 200);
    SDL_Surface *tmp= TTF_RenderText_Solid(font, text, color);
    SDL_Texture *texture= SDL_CreateTextureFromSurface(sdlRenderer, tmp);
    SDL_FreeSurface(tmp);
    tmp=NULL;
    return texture;
}

#endif //TEST_GRAPHICS_TEXTURE_H
