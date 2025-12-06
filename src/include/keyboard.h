#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "app.h"

void addKeyboardRects(appContext *app, int x, int y, int w, int h, bool down);
void handleInputKeyboardCard(appContext *app, SDL_Scancode code, bool pressing);
void handleKeyboardInput(appContext *app, SDL_Scancode code);

#endif