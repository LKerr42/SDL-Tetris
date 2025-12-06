#include "include/keyboard.h"
#include "include/app.h"

void addKeyboardRects(appContext *app, int x, int y, int w, int h, bool down) {
    SDL_SetRenderTarget(app->renderer, app->backgroundKeyboard);

    SDL_FRect rect = {
        x * KEYBOARD_PIXEL_SIZE,
        y * KEYBOARD_PIXEL_SIZE, //x and y
        w * KEYBOARD_PIXEL_SIZE, 
        h * KEYBOARD_PIXEL_SIZE, //w and h
    };

    if (down) {
        SDL_SetRenderDrawColor(app->renderer, 255, 53, 38, SDL_ALPHA_OPAQUE); 
    } else {
        SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); 
    }

    SDL_RenderFillRect(app->renderer, &rect);

    SDL_SetRenderTarget(app->renderer, NULL);
}