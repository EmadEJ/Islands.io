//
// Created by emad on 02/02/2022.
//

#ifndef TEST_GRAPHICS_TEXTURE_H
#define TEST_GRAPHICS_TEXTURE_H

SDL_Texture *getImageTexture(SDL_Renderer *sdlRenderer, char *image_path);

SDL_Texture *getTextTexture(SDL_Renderer *sdlRenderer, char *text, SDL_Color color, char *fontPath, int size);

void putImage(SDL_Renderer *sdlRenderer, char *image_path, int x, int y, int w,int h);

void putText(SDL_Renderer *sdlRenderer, char *text, SDL_Color color, char *fontPath, int size, int x, int y);

void putTextMid(SDL_Renderer *sdlRenderer, char *text, SDL_Color color, char *fontPath, int size, int y);

#endif //TEST_GRAPHICS_TEXTURE_H
