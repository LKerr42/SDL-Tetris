#ifndef RENDERER_H
#define RENDERER_H

#include <stdbool.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>

#define addOrTake(num) ((num) > (0) ? (-1) : (1))

struct appContext;

/**
 * Display a block to the main renderer.
 * 
 * \param *app  Pointer to the app context
 * \param rect  rect data for block
 * \param r     R RGB value
 * \param g     G RGB value
 * \param b     B RGB value
 */
void displayBlock(struct appContext *app, SDL_FRect rect, int r, int g, int b);

/**
 * Display a block to a texture.
 * 
 * \param *app        Pointer to the app context
 * \param rect        rect data for block
 * \param r           R RGB value
 * \param g           G RGB value
 * \param b           B RGB value
 * \param texture     texture to render to
 * \param isWireFrame Display as a wireframe or not
 */
void displayBlockToTexture(struct appContext *app, SDL_FRect rect, int r, int g, int b, SDL_Texture* texture, bool isWireframe);

/**
 * Compute the texture for the main board.
 * 
 * \param *app Pointer to the app context
 */
void buildBoardTexture(struct appContext *app);

/**
 * Render the board texture to the main window
 * 
 * \param *app Pointer to the app context
 */
void renderBoard(struct appContext *app);

/**
 * Compute the texture for the next blocks.
 * 
 * \param *app Pointer to the app context
 */
void updateNextBlocks(struct appContext *app);

/**
 * Render the next blocks texture to the main window
 * 
 * \param *app Pointer to the app context.
 */
void displayNextBlocks(struct appContext *app);

/**
 * Render the main game text to the screen.
 * 
 * \param *app Pointer to the app context.
 */
void displayMainStaticText(struct appContext *app);

/**
 * Render the static control menu text to the screen.
 * 
 * \param *app Pointer to the app context.
 */
void displayKeyboardStaticText(struct appContext *app);

/**
 * Render the static title screen text to the screen.
 * 
 * \param *app Pointer to the app context.
 */
void displayTitleStaticText(struct appContext *app);

/**
 * Render the in-game control menu popup.
 * 
 * \param *app Pointer to the app context.
 */
void displayControlsPopup(struct appContext *app);

/**
 * Render the rectangles to clear a line.
 * 
 * \param *app Pointer to the app context.
 */
void displayLineClearColumns(struct appContext *app);

/**
 * Initalise the snowflakes for the christmas theme
 * 
 * \param *app Pointer to the app context.
 */
void initSnow(struct appContext *app);

/**
 * Run the snowflakes animations for the christmas theme
 * 
 * \param *app Pointer to the app context.
 */
void updateSnow(struct appContext *app);

/**
 * Render the snowflakes for the christmas theme
 * 
 * \param *app Pointer to the app context.
 */
void renderSnow(struct appContext *app);

/**
 * Draw a border to a surface of any colour or thickness.
 * 
 * \param *app      Pointer to the app context.
 * \param *surf     Surface to render to.
 * \param thickness Thickness in pixels of the border.
 * \param r         Red RGB value for the border.
 * \param g         Green RGB value for the border.
 * \param b         Blue RGB value for the border.
 * \param a         Alpha (opacity) value for the border.
 */
void drawSurfaceBorder(struct appContext *app, SDL_Surface *surf, int thickness, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

#endif