#include "renderer.h"
#include "tetromino.h"
#include "app.h"

void buildBoardTexture(appContext *app) {
    SDL_SetRenderTarget(app->renderer, app->boardTexture);

    // Clear previous contents
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderClear(app->renderer);

    SDL_FRect rect;
    rect.w = rect.h = TETROMINO_BLOCK_SIZE;

    for (int i = 0; i < 22; i++) { 
        for (int j = 0; j < 12; j++) {
            if (app->filledBlocks[i][j]->v == true) {
                rect.x = j * TETROMINO_BLOCK_SIZE;
                rect.y = i * TETROMINO_BLOCK_SIZE;

                displayBlockToTexture(rect, app->filledBlocks[i][j]->r, app->filledBlocks[i][j]->g, app->filledBlocks[i][j]->b, app->boardTexture, false);
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
                    displayBlockToTexture(Brect, app->wireframeTet->r, app->wireframeTet->g, app->wireframeTet->b, app->boardTexture, true);
                }
            }
        }
    }

    SDL_SetRenderTarget(app->renderer, NULL);
}
