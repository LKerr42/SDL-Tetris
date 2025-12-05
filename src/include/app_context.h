#ifndef APP_CONTEXT_H
#define APP_CONTEXT_H

#include <SDL3/SDL_video.h>
#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>

#define max(X, Y) ((X) > (Y) ? (X) : (Y))
#define min(X, Y) ((X) < (Y) ? (X) : (Y))

typedef struct {
    SDL_Texture *tex;
    SDL_FRect dest;
} textTexture;

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;

    int width;
    int height;

    textTexture textArray[300];
    SDL_Texture *keyboardText;

    TTF_Font* globalFont;
    TTF_Font* globalFontS;
} appContext;

#endif