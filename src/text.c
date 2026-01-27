#include "include/text.h"
#include "include/app.h"
#include "include/renderer.h"
#include "include/stats.h"
#include "include/tetromino.h"
#include <string.h>
#include <stdlib.h>
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
        "Esc.Pause/Play  Del.Quit",
        "F1..Help        F2..Stats",
        "R...Restart     Alt.Wire",
        "             ",
        "Left/Right..Move L/R",
        "Down........Soft/Hard Drop",
        "A/D.........Rotate CW/CCW"
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

    //destroy combined
    SDL_DestroySurface(combined);
}

void buildStatsText(appContext *app) {
    char *strings[15];
    char *statStrings[13];
    char *tetDropStrings[7];

    for (int j = 0; j < arraySize(strings); j++) {
        strings[j] = calloc(1, sizeof(char));
    }

    statStrings[0] = fillIntegerStringToSize(3, app->userStats->gamesPlayed, '0');
    statStrings[1] = fillIntegerStringToSize(4, app->userStats->totalLinesCleared, '0');
    statStrings[2] = fillIntegerStringToSize(2, app->userStats->highestLevel, '0');
    statStrings[3] = fillIntegerStringToSize(4, app->userStats->totalTetrises, '0');
    statStrings[4] = fillIntegerStringToSize(7, app->userStats->highestScore, '0');
    statStrings[5] = fillIntegerStringToSize(3, app->userStats->gameLinesCleared, '0');

    for (int n = 0; n < 7; n++) {
        tetDropStrings[n] = calloc(1, sizeof(char));
        statStrings[6+n] = fillIntegerStringToSize(3, app->userStats->gameSpecTetsDropped[n], '0');
    }

    //Regular strings
    SDL_asprintf(&strings[0], "             -Statistics-");
    SDL_asprintf(&strings[1], "Games Played..%s Lines Cleared..%s", statStrings[0], statStrings[1]);
    SDL_asprintf(&strings[2], "Highest Level.%s  Total Tetrises.%s", statStrings[2], statStrings[3]);
    SDL_asprintf(&strings[3], "Highest Score.................%s", statStrings[4]);
    SDL_asprintf(&strings[5], "          -Stats this game-");
    SDL_asprintf(&strings[6], "         Tetrominoes Dropped:");
    //SDL_asprintf(&strings[8],  "      %s      %s      %s     %s", statStrings[6], statStrings[7], statStrings[8], statStrings[9]); //"      000      000      000     000"
    //SDL_asprintf(&strings[11], "          %s      %s      %s", statStrings[10], statStrings[11], statStrings[12]);  //"          000      000      000"
    SDL_asprintf(&strings[14], "         Lines Cleared..%s", statStrings[5]);

    for (int j = 0; j < arraySize(strings); j++) {
        if (strings[j][0] == '\0') {
            SDL_asprintf(&strings[j], "                   ");
        }
    }

    //Tet dropped strings
    SDL_asprintf(&tetDropStrings[0], "      %s", statStrings[6]); //same
    SDL_asprintf(&tetDropStrings[1], "      %s", statStrings[7]); //same
    SDL_asprintf(&tetDropStrings[2], "      %s", statStrings[8]); //same
    SDL_asprintf(&tetDropStrings[3], "     %s", statStrings[9]);
    SDL_asprintf(&tetDropStrings[4], "          %s", statStrings[10]);
    SDL_asprintf(&tetDropStrings[5], "      %s", statStrings[11]); //same
    SDL_asprintf(&tetDropStrings[6], "      %s", statStrings[12]); //same


    SDL_Surface* surfaces[20];
    SDL_Rect destRects[20];
    SDL_Color textColour;
    int textHeight = 0;

    //setup combined surface
    SDL_Surface *combined = SDL_CreateSurface(
        970, 
        550, 
        SDL_PIXELFORMAT_RGBA32
    );

    Uint32 bgColor = SDL_MapRGBA(
        SDL_GetPixelFormatDetails(combined->format), NULL,
        0, 0, 0, 255   // solid background
    );

    SDL_FillSurfaceRect(combined, NULL, bgColor);
    drawSurfaceBorder(app, combined, 10, 255, 255, 255, 255);

    //Main loop
    for (int i = 0; i < arraySize(strings); i++) {
        // Calculate text colour
        textColour = (SDL_Color){255, 255, 255, 255};
        if (i == 8 || i == 11) {
            textColour = (SDL_Color){255, 0, 0, 255};
        }

        // Render text to a surface
        surfaces[i] = TTF_RenderText_Blended(app->globalFont, strings[i], strlen(strings[i]), textColour);
        if (!surfaces[i]) {
            SDL_Log("TTF_RenderText_Blended Error, surface %d: %s", i, SDL_GetError());
            return;
        }

        textHeight = surfaces[i]->h;

        //setup destination rects
        destRects[i] = (SDL_Rect){
            20,
            20 + (textHeight * (i+1)),
            surfaces[i]->w,
            surfaces[i]->h
        };

        //copy to combined
        SDL_BlitSurface(surfaces[i], NULL, combined, &destRects[i]);

        SDL_DestroySurface(surfaces[i]);
    }

    SDL_Rect tetDests[7];

    int averageDropped = 0;
    for (int a = 0; a < 7; a++) {
        averageDropped += app->userStats->gameSpecTetsDropped[a];
    }
    averageDropped /= 7;
    printf("Average: %d\n", averageDropped);

    //Dropped tet loop
    for (int i = 0; i < 7; i++) {
        //Calculate colour
        int distToAvg = app->userStats->gameSpecTetsDropped[i] - averageDropped;
        printf("Dist of %d: %d\n", i, distToAvg);

        int r = (200 + distToAvg * 4 < 255) ? (200 + distToAvg * 4) : (255);
        textColour = (SDL_Color){r, 0, 0, 255};

        // Render text to a surface
        surfaces[i] = TTF_RenderText_Blended(app->globalFont, tetDropStrings[i], strlen(tetDropStrings[i]), textColour);
        if (!surfaces[i]) {
            SDL_Log("TTF_RenderText_Blended Error, tet surface %d: %s", i, SDL_GetError());
            return;
        }

        //calculate destination
        int row = (i < 4) ? 8 : 11;
        int xPos = 0;
        if (i == 4 || i == 0) {
            xPos = 20;
        } else {
            xPos = tetDests[i-1].x + surfaces[i-1]->w;
        }

        tetDests[i] = (SDL_Rect){
            xPos,
            destRects[row].y,
            surfaces[i]->w,
            surfaces[i]->h
        };

        SDL_BlitSurface(surfaces[i], NULL, combined, &tetDests[i]);
    }

    // Convert surface to texture
    float textWidthf, textHeightf;
    app->statsTexture.tex = SDL_CreateTextureFromSurface(app->renderer, combined);
    if (!app->statsTexture.tex) {
        SDL_Log("SDL_CreateTextureFromSurface Error: %s", SDL_GetError());
        return;
    }
    SDL_GetTextureSize(app->statsTexture.tex, &textWidthf, &textHeightf);

    //Draw tetrominoes
    SDL_Texture *tempTex = SDL_CreateTexture(
        app->renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        textWidthf,
        textHeightf
    );

    SDL_SetRenderTarget(app->renderer, tempTex);
    SDL_RenderTexture(app->renderer, app->statsTexture.tex, NULL, NULL);
    SDL_SetRenderTarget(app->renderer, NULL);

    int moveSide = 0, moveDown = 0, timesByMove = 0;

    //drawing loop
    for (int k = 0; k < 7; k++) {
        moveSide = (k <= 3) ? 80  : -730;
        moveDown = (k <= 3) ? 265 : 365;
        if (k <= 3 && k != 0) moveDown += 10;
        
        for (int l = 0; l < 4; l++) {
            for (int m = 0; m < 4; m++) {
                if (app->tetArray[k]->blocks[l][m].active == true) {
                    SDL_FRect Srect = {
                        m * TETROMINO_BLOCK_SIZE + moveSide + (230 * k),
                        l * TETROMINO_BLOCK_SIZE + moveDown,
                        TETROMINO_BLOCK_SIZE,
                        TETROMINO_BLOCK_SIZE
                    };

                    displayBlockToTexture(
                        app, Srect, app->tetArray[k]->r, 
                        app->tetArray[k]->g, 
                        app->tetArray[k]->b, 
                        tempTex, false
                    );                          
                }
            }
        }
    }

    SDL_DestroyTexture(app->statsTexture.tex);
    app->statsTexture.tex = tempTex;
    

    app->statsTexture.dest = (SDL_FRect){
        (app->width / 2) - (combined->w/2),
        (app->height / 2) - (combined->h/2),
        textWidthf,
        textHeightf
    };

    //destroy combined
    SDL_DestroySurface(combined);
}

void prependChar(char *str, char c) {
    size_t len = strlen(str);

    memmove(str + 1, str, len + 1);

    str[0] = c;
}

char* fillIntegerStringToSize(int length, int value, char c) {
    char *str = calloc(1, sizeof(char));
    SDL_asprintf(&str, "%d", value);
    size_t startLength = strlen(str);

    while (startLength != length) {
        prependChar(str, c);
        startLength++;
    }

    return str;
}