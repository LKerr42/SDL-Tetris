#include "renderer.h"
#include "tetromino.h"
#include "app.h"

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

//TODO: improve efficency by moving rect def to widow resize
void renderBoard(appContext *app) {
    SDL_FRect displayRect = {
        app->bWidthMin,
        app->bHeightMin,
        12*TETROMINO_BLOCK_SIZE,
        22*TETROMINO_BLOCK_SIZE
    };

    SDL_RenderTexture(app->renderer, app->boardTexture, NULL, &displayRect);
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