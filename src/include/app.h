#ifndef APP_H
#define APP_H

#include <SDL3/SDL_video.h>
#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>

/**
 * Pixel size of all blocks
 */
#define TETROMINO_BLOCK_SIZE 20
/**
 * Pixel size of the keyboard texture
 */
#define KEYBOARD_PIXEL_SIZE  10

/**
 * Returns the max of two inputs
 */
#define max(X, Y) ((X) > (Y) ? (X) : (Y))
/**
 * Returns the min of two inputs
 */
#define min(X, Y) ((X) < (Y) ? (X) : (Y))

/**
 * textTexture:
 * Represents an instance of a texture bundled with a destination rect
 */
typedef struct {
    SDL_Texture *tex;
    SDL_FRect dest;
} textTexture;

/**
 * appContext:
 * Represents the main context for the entire app, only one instance.
 * Contains all related to rendering, the window, sizes, textures, and fonts
 */
typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;

    int width, height;
    int bWidthMin, bWidthMax, bHeightMin, bHeightMax;
    float textW, textH;

    textTexture textArray[300];
    SDL_Texture *keyboardText;

    SDL_Texture *staticText;
    SDL_Texture *backgroundKeyboard;

    TTF_Font* globalFont;
    TTF_Font* globalFontS;
} appContext;

void prependChar(char *str, char c);

#endif