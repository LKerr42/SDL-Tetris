#ifndef TEXT_H
#define TEXT_H

#include <SDL3_ttf/SDL_ttf.h>
#include "app_context.h"

void displayText(appContext *app, char str[], int x, int y, TTF_Font* font, int r, int g, int b);
void writeToKeyboardText(appContext *app, char mainStr[], char subStr[], bool write, SDL_Scancode scancode);

#endif