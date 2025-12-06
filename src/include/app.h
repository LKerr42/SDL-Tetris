#ifndef APP_H
#define APP_H

#include <SDL3/SDL_video.h>
#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "tetromino.h"

/**
 * Pixel size of all blocks
 */
#define TETROMINO_BLOCK_SIZE 20
/**
 * Pixel size of the keyboard texture
 */
#define KEYBOARD_PIXEL_SIZE  10

#define CLEAR     0 
#define HOLYMOLY  1
#define LAND      2
#define MOVELEFT  3
#define MOVERIGHT 4
#define OPEN      5
#define SPINCCW   6
#define SPINCW    7
#define SWITCH    8

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

    bool winning, keyboardCard, titleCard, showWireframe;

    Uint8 amountPressed;
    Uint8 amountPressedDown;

    textTexture textArray[300];
    SDL_Texture *keyboardText;

    SDL_Texture *staticText;
    SDL_Texture *backgroundKeyboard;

    TTF_Font* globalFont;
    TTF_Font* globalFontS;

    tetromino currentTet;
    tetromino tetArray[7];
    int nextBlocks[4];
    int heldtet, currentBlock;
} appContext;

/**
 * Place a character at the begining of a string.
 * 
 * \param *str Pointer to the string to modify
 * \param c    Character to add
 */
void prependChar(char *str, char c);

#endif