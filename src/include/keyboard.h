#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "app.h"

/**
 * Renders the rect appropriate when a key is pressed
 * 
 * \param *app  Pointer to the main context
 * \param x     X value (in number of keyboard pixels) to display rect
 * \param y     Y value (in number of keyboard pixels) to display rect
 * \param w     Width value (in number of keyboard pixels) to display rect
 * \param h     height value (in number of keyboard pixels) to display rect
 * \param down  True: display rect. False: clear rect
 */
void addKeyboardRects(appContext *app, int x, int y, int w, int h, bool down);

/**
 * Handle the inputs when on the keyboard card
 * 
 * \param *app     Pointer to the main context
 * \param code     Scancode of the key pressed
 * \param pressing True/false on up/down press
 */
void handleInputKeyboardCard(appContext *app, SDL_Scancode code, bool pressing);

/**
 * Handle the inputs when playing the main game
 * 
 * \param *app     Pointer to the main context
 * \param code     Scancode of the key pressed
 */
void handleKeyboardInput(appContext *app, SDL_Scancode code);

#endif