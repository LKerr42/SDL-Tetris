#include "include/text.h"
#include "include/app.h"
#include <string.h>
#include <stdio.h>

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

void setupStaticText(appContext *app) {
    //board sizes 
    int widMinT = app->bWidthMin, widMaxT = app->bWidthMax;
    int higMinT = app->bHeightMin, higMaxT = app->bHeightMax;

    // Render text to a surface
    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface* surface1 = TTF_RenderText_Blended(app->globalFont, "Tetris Time!", 12, textColor);
    SDL_Surface* surface2  = TTF_RenderText_Blended(app->globalFont, "-Held-", 6, textColor);
    SDL_Surface* surface3  = TTF_RenderText_Blended(app->globalFont, "-Score-", 7, textColor);
    SDL_Surface* surface4  = TTF_RenderText_Blended(app->globalFont, "-Next Blocks-", 13, textColor);
    if (!surface1 || !surface2 || !surface3) {
        SDL_Log("TTF_RenderText_Blended Error: %s", SDL_GetError());
        return;
    }

    int textHeight = surface1->h;

    SDL_Surface *combined = SDL_CreateSurface(widMaxT+60+surface4->w, higMinT+(textHeight*3)+60, SDL_PIXELFORMAT_RGBA32);

    //setup destination rects
    SDL_Rect dest1 = {(app->width >> 1) - (surface1->w >> 1) + 15, higMinT>>1, surface1->w, textHeight};
    SDL_Rect dest2 = {widMinT-180, higMinT+20, surface2->w, textHeight};
    SDL_Rect dest3 = {widMaxT+20, higMinT+20, surface3->w, textHeight};
    SDL_Rect dest4 = {widMaxT+20, higMinT+(textHeight << 1)+60, surface4->w, textHeight};

    //copy to combined
    SDL_BlitSurface(surface1, NULL, combined, &dest1);
    SDL_BlitSurface(surface2, NULL, combined, &dest2);
    SDL_BlitSurface(surface3, NULL, combined, &dest3);
    SDL_BlitSurface(surface4, NULL, combined, &dest4);

    // Convert surface to texture
    app->staticText = SDL_CreateTextureFromSurface(app->renderer, combined);
    if (!app->staticText) {
        SDL_Log("SDL_CreateTextureFromSurface Error: %s", SDL_GetError());
        return;
    }

    SDL_GetTextureSize(app->staticText, &app->textW, &app->textH);

    //destroy
    SDL_DestroySurface(surface1);
    SDL_DestroySurface(surface2);
    SDL_DestroySurface(surface3);
    SDL_DestroySurface(surface4);
    SDL_DestroySurface(combined);
}

void updateScoreTexture(appContext *app) {
    char incompleteScore[7];
    //update string
    sprintf(incompleteScore, "%d", app->score);
    while (strlen(incompleteScore) != 7) {
        prependChar(incompleteScore, '0');
    }
    strcpy(app->scoreString, incompleteScore);

    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface* textSurface = TTF_RenderText_Blended(app->globalFont, app->scoreString, strlen(app->scoreString), textColor);
    if (!textSurface) {
        SDL_Log("TTF_RenderText_Blended Error: %s", SDL_GetError());
    }

    // Convert surface to texture
    SDL_DestroyTexture(app->scoreTexture.tex);
    app->scoreTexture.tex = SDL_CreateTextureFromSurface(app->renderer, textSurface);
    SDL_DestroySurface(textSurface); // free surface memory
    if (!app->scoreTexture.tex) {
        SDL_Log("SDL_CreateTextureFromSurface Error: %s", SDL_GetError());
        return;
    }

    // Get text dimensions
    float textW, textH;
    SDL_GetTextureSize(app->scoreTexture.tex, &textW, &textH);
    SDL_FRect temp = {
        app->scoreTexture.dest.x,
        app->scoreTexture.dest.y,
        textW,
        textH
    };
    app->scoreTexture.dest = temp;
}