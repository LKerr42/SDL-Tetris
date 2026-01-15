#include "include/renderer.h"
#include "include/tetromino.h"
#include "include/app.h"
#include <stdio.h>

void displayBlock(struct appContext *app, SDL_FRect rect, int r, int g, int b) {
    int i, j, cX = 0, cY = 0;
    int minX = rect.x, maxX = rect.w+rect.x, minY = rect.y, maxY = rect.h+rect.y;
    
    int cR = SDL_clamp(r-45, 0, 255);
    int cG = SDL_clamp(g-45, 0, 255);
    int cB = SDL_clamp(b-45, 0, 255);

    int bR = SDL_clamp(r-90, 0, 255);
    int bG = SDL_clamp(g-90, 0, 255);
    int bB = SDL_clamp(b-90, 0, 255);

    SDL_FRect pixelRect;
    pixelRect.w = pixelRect.h = 1;
    for (i = minY; i < maxY; i++) {
        cY++;
        for (j = minX; j < maxX; j++) { 
            cX++;
            if (i > minY+3 && i < maxY-3 && j > minX+3 && j < maxX-3) {
                SDL_SetRenderDrawColor(app->renderer, cR, cG, cB, SDL_ALPHA_OPAQUE); 
            } else if ((-cX+rect.w) - cY < -1) {
                SDL_SetRenderDrawColor(app->renderer, bR, bG, bB, SDL_ALPHA_OPAQUE); 
            } else {
                SDL_SetRenderDrawColor(app->renderer, r, g, b, SDL_ALPHA_OPAQUE);
            }
            pixelRect.y = i;
            pixelRect.x = j;
            SDL_RenderFillRect(app->renderer, &pixelRect);
        }
        cX = 0;
    }
}

void displayBlockToTexture(struct appContext *app, SDL_FRect rect, int r, int g, int b, SDL_Texture* texture, bool isWireframe) {
    int i, j, cX = 0, cY = 0;
    int minX = rect.x, maxX = rect.w+rect.x, minY = rect.y, maxY = rect.h+rect.y;
    
    int cR = SDL_clamp(r-45, 0, 255);
    int cG = SDL_clamp(g-45, 0, 255);
    int cB = SDL_clamp(b-45, 0, 255);

    int bR = SDL_clamp(r-90, 0, 255);
    int bG = SDL_clamp(g-90, 0, 255);
    int bB = SDL_clamp(b-90, 0, 255);

    SDL_SetRenderTarget(app->renderer, texture);

    SDL_FRect pixelRect;
    pixelRect.w = pixelRect.h = 1;
    for (i = minY; i < maxY; i++) {
        cY++;
        for (j = minX; j < maxX; j++) { 
            cX++;
            if (i > minY+3 && i < maxY-3 && j > minX+3 && j < maxX-3) {
                if (!isWireframe) {
                    SDL_SetRenderDrawColor(app->renderer, cR, cG, cB, SDL_ALPHA_OPAQUE); 
                } else {
                    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); 
                }
            } else if ((-cX+rect.w) - cY < -1) {
                SDL_SetRenderDrawColor(app->renderer, bR, bG, bB, SDL_ALPHA_OPAQUE); 
            } else {
                SDL_SetRenderDrawColor(app->renderer, r, g, b, SDL_ALPHA_OPAQUE);
            }
            pixelRect.y = i;
            pixelRect.x = j;
            SDL_RenderFillRect(app->renderer, &pixelRect);
        }
        cX = 0;
    }

    SDL_SetRenderTarget(app->renderer, NULL);
}

void buildBoardTexture(appContext *app) {
    SDL_SetRenderTarget(app->renderer, app->boardTexture);

    // Clear previous contents
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderClear(app->renderer);

    SDL_FRect rect;
    rect.w = rect.h = TETROMINO_BLOCK_SIZE;

    for (int i = 0; i < 22; i++) { 
        for (int j = 0; j < 12; j++) {
            if (app->filledBlocks[i][j].v == true) {
                rect.x = j * TETROMINO_BLOCK_SIZE;
                rect.y = i * TETROMINO_BLOCK_SIZE;

                displayBlockToTexture(
                    app, rect, 
                    app->filledBlocks[i][j].r, 
                    app->filledBlocks[i][j].g, 
                    app->filledBlocks[i][j].b, 
                    app->boardTexture, false
                );
            }
        }
    }
    if (app->showWireframe) {
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if (app->wireframeTet->blocks[y][x].active) {
                    int posX = app->wireframeTet->x + x;
                    int posY = app->wireframeTet->y + y;
                    SDL_FRect Brect = {
                        posX * TETROMINO_BLOCK_SIZE,
                        posY * TETROMINO_BLOCK_SIZE,
                        TETROMINO_BLOCK_SIZE,
                        TETROMINO_BLOCK_SIZE
                    };
                    displayBlockToTexture(
                        app, Brect, 
                        app->wireframeTet->r, 
                        app->wireframeTet->g, 
                        app->wireframeTet->b, 
                        app->boardTexture, true
                    );
                }
            }
        }
    }

    SDL_SetRenderTarget(app->renderer, NULL);
}

void renderBoard(appContext *app) {
    SDL_RenderTexture(app->renderer, app->boardTexture, NULL, &app->displayRect);
}

void displayNextBlocks(appContext *app) {
    SDL_FRect displayRect = {
        (app->bWidthMax + 20),
        (app->bHeightMin + 150),
        TETROMINO_BLOCK_SIZE << 2,
        (TETROMINO_BLOCK_SIZE << 3) + (TETROMINO_BLOCK_SIZE << 2)
    };

    SDL_RenderTexture(app->renderer, app->nextTexture, NULL, &displayRect);
}

void updateNextBlocks(appContext *app) {
    SDL_SetRenderTarget(app->renderer, app->nextTexture);

    // Clear previous contents
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderClear(app->renderer);

    int posX, posY;

    //display next blocks
    for (int block = 0; block < 4; block++) {
        for (int d = 0; d < 4; d++) {
            for (int e = 0; e < 4; e++) {
                if (app->tetArray[app->nextBlocks[block]]->blocks[d][e].active == true) {
                    posX = e * TETROMINO_BLOCK_SIZE;
                    posY = d * TETROMINO_BLOCK_SIZE + ((TETROMINO_BLOCK_SIZE*3) * block);
                    SDL_FRect Nrect = {
                        posX,
                        posY,
                        TETROMINO_BLOCK_SIZE,
                        TETROMINO_BLOCK_SIZE
                    };
                    displayBlockToTexture(
                        app, Nrect, 
                        app->tetArray[app->nextBlocks[block]]->r, 
                        app->tetArray[app->nextBlocks[block]]->g, 
                        app->tetArray[app->nextBlocks[block]]->b, 
                        app->nextTexture, false
                    );
                }
            }
        }
    }

    SDL_SetRenderTarget(app->renderer, NULL); // back to screen
}

void displayMainStaticText(appContext *app) {
    // Get text dimensions
    SDL_FRect textRect = {0, 0, app->textW, app->textH};
    SDL_RenderTexture(app->renderer, app->staticText, NULL, &textRect);
}

void displayKeyboardStaticText(appContext *app) {
    // Get text dimensions
    SDL_FRect textRect = {0, 0, app->keyTextW, app->keyTextH};
    SDL_RenderTexture(app->renderer, app->staticKeyboardText, NULL, &textRect);
}

void displayTitleStaticText(appContext *app) {
    // Get text dimensions
    SDL_FRect textRect = {0, 0, app->titleTextW, app->titleTextH};
    SDL_RenderTexture(app->renderer, app->staticTitleText, NULL, &textRect);
}

void displayControlsPopup(struct appContext *app) {
    SDL_RenderTexture(app->renderer, app->staticControlsText.tex, NULL, &app->staticControlsText.dest);
}

void displayLineClearColumns(appContext *app) {
    SDL_SetRenderTarget(app->renderer, app->boardTexture);
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);

    SDL_FRect columnRect = {
        TETROMINO_BLOCK_SIZE * app->clearInst.column,
        TETROMINO_BLOCK_SIZE, //* app->clearInst.rows[1],
        TETROMINO_BLOCK_SIZE, 
        TETROMINO_BLOCK_SIZE
    };

    for (int i = 0; i < app->clearInst.amountLines; i++) {
        columnRect.y = TETROMINO_BLOCK_SIZE * app->clearInst.rows[i+1];
        SDL_RenderFillRect(app->renderer, &columnRect);
    }

    SDL_SetRenderTarget(app->renderer, NULL);
}

void initSnow(appContext *app) {
    if (!app->Xmas) return;
    for (int i = 0; i < MAX_SNOW; i++) {
        app->snow[i].x = (int)SDL_rand(app->width);
        app->snow[i].y = (int)SDL_rand(app->height / 2);
        app->snow[i].speed = (int)SDL_rand(15) + 1;
        app->snow[i].drift = (int)SDL_rand(3);
        app->snow[i].size = (int)SDL_rand(3) + 3;
    }
}

void updateSnow(appContext *app) {
    if (!app->Xmas) return; 
    for (int i = 0; i < MAX_SNOW; i++) {
        app->snow[i].y += app->snow[i].speed;
        app->snow[i].x += addOrTake((int)SDL_rand(2)) + app->snow[i].drift;

        if (app->snow[i].y > app->height) {
            app->snow[i].y = 0;
            app->snow[i].x = (int)SDL_rand(app->width);
        }
    }
}

void renderSnow(appContext *app) {
    if (!app->Xmas) return;
    SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, 255);

    for (int i = 0; i < MAX_SNOW; i++) {
        SDL_FRect s = {
            app->snow[i].x, 
            app->snow[i].y,
            app->snow[i].size,
            app->snow[i].size
        };
        SDL_RenderFillRect(app->renderer, &s);
    }
}

void drawSurfaceBorder(appContext *app, SDL_Surface *surf, int thickness, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    Uint32 color = SDL_MapRGBA(
        SDL_GetPixelFormatDetails(surf->format), NULL, 
        r, g, b, a
    );

    SDL_Rect top    = {0, 0, surf->w, thickness};
    SDL_Rect bottom = {0, surf->h - thickness, surf->w, thickness};
    SDL_Rect left   = {0, 0, thickness, surf->h};
    SDL_Rect right  = {surf->w - thickness, 0, thickness, surf->h};

    SDL_FillSurfaceRect(surf, &top, color);
    SDL_FillSurfaceRect(surf, &bottom, color);
    SDL_FillSurfaceRect(surf, &left, color);
    SDL_FillSurfaceRect(surf, &right, color);
}