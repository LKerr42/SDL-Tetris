#include "include/text.h"
#include "include/app_context.h"

void displayText(appContext *app, char str[], int x, int y, TTF_Font* font, int r, int g, int b) {
    // Render text to a surface
    SDL_Color textColor = {r, g, b, 255};
    SDL_Surface *textSurface = TTF_RenderText_Blended(font, str, strlen(str), textColor);
    if (!textSurface) {
        SDL_Log("TTF_RenderText_Blended Error: %s", SDL_GetError());
        return;
    }

    // Convert surface to texture
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(app->renderer, textSurface);
    SDL_DestroySurface(textSurface); // free surface memory

    if (!textTexture) {
        SDL_Log("SDL_CreateTextureFromSurface Error: %s", SDL_GetError());
        return;
    }

    // Get text dimensions
    float textW, textH;
    SDL_GetTextureSize(textTexture, &textW, &textH);

    //set if set to centre
    if (x == -1) {
        x = (app->width >> 1) - (textW/2);
    }
    if (y == -1) {
        y = (app->height >> 1) - (textH/2);
    }

    SDL_FRect textRect = {x, y, textW, textH};
    SDL_RenderTexture(app->renderer, textTexture, NULL, &textRect);
    SDL_DestroyTexture(textTexture);
}

void writeToKeyboardText(appContext *app, char mainStr[], char subStr[], bool write, SDL_Scancode scancode) {
    SDL_SetRenderTarget(app->renderer, app->keyboardText);
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderClear(app->renderer);

    if (write) {
        //create surface from string and draw it to the main texture at the centre Y
        if (app->textArray[scancode].tex == NULL) {
            SDL_Color textColor = {255, 255, 255, 255};
            SDL_Surface *textSurface = TTF_RenderText_Blended(app->globalFont, mainStr, strlen(mainStr), textColor);
            if (!textSurface) {
                SDL_Log("TTF_RenderText_Blended Error: %s", SDL_GetError());
                SDL_SetRenderTarget(app->renderer, NULL);
                return;
            }
            if (subStr[0] != '\0') {
                SDL_Surface *subTextSurface = TTF_RenderText_Blended(app->globalFontS, subStr, strlen(subStr), textColor);
                SDL_Surface *combined = SDL_CreateSurface(
                    max(subTextSurface->w, textSurface->w), 
                    textSurface->h + subTextSurface->h + 10, 
                    SDL_PIXELFORMAT_RGBA32
                );

                SDL_Rect mainDest = {(combined->w - textSurface->w) / 2, 0, textSurface->w, textSurface->h};
                SDL_Rect subDest = {(combined->w - subTextSurface->w) / 2, textSurface->h + 10, subTextSurface->w, subTextSurface->h};

                SDL_BlitSurface(textSurface, NULL, combined, &mainDest);
                SDL_BlitSurface(subTextSurface, NULL, combined, &subDest);

                app->textArray[scancode].dest.w = combined->w;
                app->textArray[scancode].dest.h = combined->h;
                app->textArray[scancode].dest.x = (840 - app->textArray[scancode].dest.w) / 2;
                app->textArray[scancode].dest.y = 0;

                app->textArray[scancode].tex = SDL_CreateTextureFromSurface(app->renderer, combined);

                if (!app->textArray[scancode].tex) {
                    SDL_Log("SDL_CreateTextureFromSurface Error: %s", SDL_GetError());
                    SDL_SetRenderTarget(app->renderer, NULL);
                    return;
                }
            } else {
                app->textArray[scancode].dest.w = textSurface->w;
                app->textArray[scancode].dest.h = textSurface->h;
                app->textArray[scancode].dest.x = (840 - app->textArray[scancode].dest.w) / 2;
                app->textArray[scancode].dest.y = 0;
                
                app->textArray[scancode].tex = SDL_CreateTextureFromSurface(app->renderer, textSurface);

                if (!app->textArray[scancode].tex) {
                    SDL_Log("SDL_CreateTextureFromSurface Error: %s", SDL_GetError());
                    SDL_SetRenderTarget(app->renderer, NULL);
                    return;
                }
            }
            
            SDL_RenderTexture(app->renderer, app->textArray[scancode].tex, NULL, &app->textArray[scancode].dest);
            SDL_DestroySurface(textSurface); 
        } else {
            SDL_RenderTexture(app->renderer, app->textArray[scancode].tex, NULL, &app->textArray[scancode].dest);
        }

    }

    SDL_SetRenderTarget(app->renderer, NULL);
}