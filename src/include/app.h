#ifndef APP_H
#define APP_H

#include <SDL3/SDL_video.h>
#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>

typedef struct tetromino tetromino;

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
 * setBlocks:
 * Data for rendering the board. The board is made up fo a 12x22 grid of these.
 * Here because they break when used as pointers ngl.
 */
typedef struct setBlocks {
    bool v;
    int r;
    int g;
    int b;
} setBlocks;

/**
 * appContext:
 * Represents the main context for the entire app, only one instance.
 * Contains all related to rendering, the window, sizes, textures, tetrominoes, and fonts
 */
typedef struct appContext {
    //Pointers to the window and renderer
    SDL_Window *window;
    SDL_Renderer *renderer;

    int width, height; //Width and height of the window in pixels
    int bWidthMin, bWidthMax, bHeightMin, bHeightMax; //bounding box of the board in pixels
    float textW, textH; // Width and height of the static text

    //Control booleans
    bool winning, keyboardCard, titleCard, showWireframe, paused;

    //Double press control counters
    Uint8 amountPressed, amountPressedDown;

    //array of all the text textures in the control menu and its main texture
    textTexture textArray[300];
    SDL_Texture *keyboardText;

    //static text and red backgrounds for keyboard texture
    SDL_Texture *staticText;
    SDL_Texture *backgroundKeyboard;

    //textures
    SDL_Texture *boardTexture;
    SDL_Texture *nextTexture;

    //Three sizes of the global font
    TTF_Font* globalFont;
    TTF_Font* globalFontS;
    TTF_Font* globalFontL;

    //Pointers to the uses of tetromino struct
    tetromino *currentTet;
    tetromino *tetArray[7];
    tetromino *titleTetroes[6];
    tetromino *wireframeTet;

    //Control ints for next, held, and current tetroes
    int nextBlocks[4];
    int heldtet, currentBlock;

    //Filled blocks data
    setBlocks filledBlocks[22][12];
} appContext;

/**
 * Place a character at the begining of a string.
 * 
 * \param *str Pointer to the string to modify
 * \param c    Character to add
 */
void prependChar(char *str, char c);

bool canMove(tetromino *t, int dx, int dy);
void runWireframes(tetromino *copyTet);
void rotateTetrominoCCW(tetromino *t);
void rotateTetrominoCW(tetromino *t);
void resetGame();

#endif