#include "include/text.h"
#include "include/app.h"
#include "include/renderer.h"
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

void setupMainStaticText(appContext *app) {
    //board sizes 
    int widMinT = app->bWidthMin, widMaxT = app->bWidthMax;
    int higMinT = app->bHeightMin, higMaxT = app->bHeightMax;

    // Render text to a surface
    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface* surface1 = TTF_RenderText_Blended(app->globalFont, "Tetris Time!", 12, textColor);
    SDL_Surface* surface2  = TTF_RenderText_Blended(app->globalFont, "-Held-", 6, textColor);
    SDL_Surface* surface3  = TTF_RenderText_Blended(app->globalFont, "-Score-", 7, textColor);
    SDL_Surface* surface4  = TTF_RenderText_Blended(app->globalFont, "-Next-", 6, textColor);
    if (!surface1 || !surface2 || !surface3 || !surface4) {
        SDL_Log("TTF_RenderText_Blended Error: %s", SDL_GetError());
        return;
    }

    int textHeight = surface1->h;

    SDL_Surface *combined = SDL_CreateSurface(widMaxT+60+surface4->w, higMinT+(textHeight*3)+60, SDL_PIXELFORMAT_RGBA32);

    //setup destination rects
    SDL_Rect dest1 = {
        (app->width >> 1) - (surface1->w >> 1), 
        (higMinT >> 1) - textHeight, 
        surface1->w, 
        textHeight
    };

    SDL_Rect dest2 = {widMinT-180, higMinT+20, surface2->w, textHeight};
    SDL_Rect dest3 = {widMaxT+20, higMinT+20, surface3->w, textHeight};
    SDL_Rect dest4 = {widMaxT+20, higMinT+(textHeight << 1)+50, surface4->w, textHeight};

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

void setupKeyboardStaticText(appContext *app) {
    // Render text to a surface
    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface* surface1 = TTF_RenderText_Blended(app->globalFont, "-- Controls menu --", 19, textColor);
    SDL_Surface* surface2  = TTF_RenderText_Blended(app->globalFontS, "Double space to start, F1 to see controls in-game", 49, textColor);
    if (!surface1 || !surface2) {
        SDL_Log("TTF_RenderText_Blended Error: %s", SDL_GetError());
        return;
    }

    int textHeight = surface1->h;

    //setup destination rects
    SDL_Rect dest1 = {
        (app->width / 2) - (surface1->w / 2), 
        10, 
        surface1->w, 
        textHeight
    };
    SDL_Rect dest2 = {
        (app->width / 2) - (surface2->w / 2), 
        50, 
        surface2->w, 
        surface2->h
    };

    //setup combined surface
    SDL_Surface *combined = SDL_CreateSurface(
        (app->width / 2) + (surface2->w / 2), 
        50+surface2->h, 
        SDL_PIXELFORMAT_RGBA32
    );

    //copy to combined
    SDL_BlitSurface(surface1, NULL, combined, &dest1);
    SDL_BlitSurface(surface2, NULL, combined, &dest2);

    // Convert surface to texture
    app->staticKeyboardText = SDL_CreateTextureFromSurface(app->renderer, combined);
    if (!app->staticKeyboardText) {
        SDL_Log("SDL_CreateTextureFromSurface Error: %s", SDL_GetError());
        return;
    }

    SDL_GetTextureSize(app->staticKeyboardText, &app->keyTextW, &app->keyTextH);

    //destroy
    SDL_DestroySurface(surface1);
    SDL_DestroySurface(surface2);
    SDL_DestroySurface(combined);
}

void setupTitleStaticText(appContext *app) {
    // Render text to a surface
    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface* surface1 = TTF_RenderText_Blended(app->globalFont, "Tribute by LKerr42", 18, textColor);
    SDL_Surface* surface2  = TTF_RenderText_Blended(app->globalFont, "Press Space to start", 20, textColor);
    if (!surface1 || !surface2) {
        SDL_Log("TTF_RenderText_Blended Error: %s", SDL_GetError());
        return;
    }

    int textHeight = surface1->h;

    //setup destination rects
    SDL_Rect dest1 = {
        (app->width / 2) - (surface1->w / 2), 
        (app->height >> 1) + (TETROMINO_BLOCK_SIZE << 1), 
        surface1->w, 
        textHeight
    };
    SDL_Rect dest2 = {
        (app->width / 2) - (surface2->w / 2), 
        10, 
        surface2->w, 
        textHeight
    };

    //setup combined surface
    SDL_Surface *combined = SDL_CreateSurface(
        (app->width / 2) + (surface2->w / 2), 
        (app->height >> 1) + (TETROMINO_BLOCK_SIZE << 1) + textHeight, 
        SDL_PIXELFORMAT_RGBA32
    );

    //copy to combined
    SDL_BlitSurface(surface1, NULL, combined, &dest1);
    SDL_BlitSurface(surface2, NULL, combined, &dest2);

    // Convert surface to texture
    app->staticTitleText = SDL_CreateTextureFromSurface(app->renderer, combined);
    if (!app->staticTitleText) {
        SDL_Log("SDL_CreateTextureFromSurface Error: %s", SDL_GetError());
        return;
    }

    SDL_GetTextureSize(app->staticTitleText, &app->titleTextW, &app->titleTextH);

    //destroy
    SDL_DestroySurface(surface1);
    SDL_DestroySurface(surface2);
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
    //TODO: could be put behind a if null statement
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

void updateLevelTexture(appContext *app) {
    char levelString[9];
    if (app->level < 10) {
        sprintf(levelString, "Level: 0%d", app->level);
    } else {
        sprintf(levelString, "Level: %d", app->level);
    }
    
    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface* textSurface = TTF_RenderText_Blended(app->globalFont, levelString, strlen(levelString), textColor);
    if (!textSurface) {
        SDL_Log("TTF_RenderText_Blended Error: %s", SDL_GetError());
    }

    // Convert surface to texture
    SDL_DestroyTexture(app->levelTexture.tex);
    app->levelTexture.tex = SDL_CreateTextureFromSurface(app->renderer, textSurface);
    SDL_DestroySurface(textSurface); // free surface memory
    if (!app->levelTexture.tex) {
        SDL_Log("SDL_CreateTextureFromSurface Error: %s", SDL_GetError());
        return;
    }

    float textW, textH;
    SDL_GetTextureSize(app->levelTexture.tex, &textW, &textH);
    SDL_FRect temp = {
        app->levelTexture.dest.x,
        app->levelTexture.dest.y,
        textW,
        textH
    };
    app->levelTexture.dest = temp;
}

void initControlsText(appContext *app) {
    char strings[8][27] = {
        "        -controls-",
        "Esc Pause/Play  Del Quit",
        "F1  Help        F2  Stats",
        "R   Restart     Alt Wire",
        "             ",
        "Left/Right  Move L/R",
        "Down        Soft/Hard Drop",
        "A/D         Rotate CW/CCW"
    };
    SDL_Surface* surfaces[8];
    SDL_Rect destRects[8];
    SDL_Color textColor = {255, 255, 255, 255};
    int textHeight = 0;

    //setup combined surface
    SDL_Surface *combined = SDL_CreateSurface(
        800, 
        500, 
        SDL_PIXELFORMAT_RGBA32
    );

    Uint32 bgColor = SDL_MapRGBA(
        SDL_GetPixelFormatDetails(combined->format), NULL,
        0, 0, 0, 255   // solid background
    );

    SDL_FillSurfaceRect(combined, NULL, bgColor);

    drawSurfaceBorder(app, combined, 10, 255, 255, 255, 255);

    for (int i = 0; i < 8; i++) {
        // Render text to a surface
        surfaces[i] = TTF_RenderText_Blended(app->globalFont, strings[i], strlen(strings[i]), textColor);
        if (!surfaces[i]) {
            SDL_Log("TTF_RenderText_Blended Error, surface %d: %s", i, SDL_GetError());
            return;
        }

        textHeight = surfaces[i]->h;

        //setup destination rects
        destRects[i] = (SDL_Rect){
            75,
            40 + (textHeight * (i+1)),
            surfaces[i]->w,
            surfaces[i]->h
        };

        //copy to combined
        SDL_BlitSurface(surfaces[i], NULL, combined, &destRects[i]);

        SDL_DestroySurface(surfaces[i]);
    }

    // Convert surface to texture
    float textWidthf, textHeightf;
    app->staticControlsText.tex = SDL_CreateTextureFromSurface(app->renderer, combined);
    if (!app->staticControlsText.tex) {
        SDL_Log("SDL_CreateTextureFromSurface Error: %s", SDL_GetError());
        return;
    }
    SDL_GetTextureSize(app->staticControlsText.tex, &textWidthf, &textHeightf);

    app->staticControlsText.dest = (SDL_FRect){
        (app->width / 2) - 400,
        (app->height / 2) - 250,
        textWidthf,
        textHeightf
    };

    /*
    //draw border
    SDL_SetRenderTarget(app->renderer, app->staticControlsText.tex);
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); 

    SDL_FRect border = {
        0, 
        0,
        textWidthf,
        textHeightf
    };

    SDL_RenderRect(app->renderer, &border);
    */

    //destroy combined
    SDL_DestroySurface(combined);
}