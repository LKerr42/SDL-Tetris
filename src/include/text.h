#ifndef TEXT_H
#define TEXT_H

#include <SDL3_ttf/SDL_ttf.h>
#include "app.h"

/**
 * Displays a string to the board with the specified params
 * Not the recomended function to use, but can be used for pages where performance does not matter
 * 
 * \param *app  Pointer to the main context
 * \param str[] String to display
 * \param x     X co-ord to display at
 * \param y     Y co-ord to display at
 * \param font  Pointer to the font to use
 * \param r     Red rgb value for the text
 * \param g     Green rgb value for the text
 * \param b     Blue rgb value for the text
 */
void displayText(appContext *app, char str[], int x, int y, TTF_Font* font, int r, int g, int b);

/**
 * Creates and stores a texture with the text for the main control menu. Creates the texture once and then displays it while held down
 * Much more readable and efficent than creating all the textures in say app_init
 * 
 * \param *app      Pointer to the main context
 * \param mainStr[] Main text to display, should contain the name of the key
 * \param subStr[]  Sub text to display, should describe the function of the key
 * \param write     Pass in a false when key is up to wipe the main texture, else true
 * \param scancode  Integer scancode value to index texture within the array clearly
 */
void writeToKeyboardText(appContext *app, char mainStr[], char subStr[], bool write, SDL_Scancode scancode);

/**
 * Setup the staic text for the main game, rerun whenever the window changes size
 * 
 * \param *app Pointer to the main context
 */
void setupStaticText(appContext *app);

#endif