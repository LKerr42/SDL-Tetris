#include "include/game_control.h"
#include "include/app.h"
#include "include/tetromino.h"
#include "include/renderer.h"

void resetBoard(appContext *app) {
    for (int i = 0; i < 22; i++) {
        for (int j = 0; j < 12; j++) {
            if (i == 0 || i == 21 || j == 0 || j == 11) {
                app->filledBlocks[i][j].v = true;
                app->filledBlocks[i][j].r = app->filledBlocks[i][j].g = app->filledBlocks[i][j].b = 125;
            } else {
                app->filledBlocks[i][j].v = false;
            }
        }
    }
    buildBoardTexture(app);
    updateNextBlocks(app);
    displayNextBlocks(app);
    renderBoard(app);
}

void resetGame(appContext *app) {
    resetBoard(app);
    for (int i = 0; i < 7; i++) {
        app->scoreString[i] = '0';
        if (i < 4) {
            app->nextBlocks[i] = SDL_rand(7);
        }
    }
    app->score = 0;
    app->heldtet = -1;
    app->currentBlock = 0;
    app->currentTet->x = 4;
    app->currentTet->y = 1;
    app->winning = true;
    app->loseCard = false;
    app->firstRun = true;
}

void runWireframes(appContext *app, tetromino *copyTet) {
    if (app->showWireframe) {
        *app->wireframeTet = *copyTet;
        while (true) {
            if (canMove(app, app->wireframeTet, 0, 1)) {
                app->wireframeTet->y += 1;
            } else {
                buildBoardTexture(app);
                break;
            }
        }
    }
}

bool canMove(appContext *app, tetromino *t, int dx, int dy) {
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            blockStruct *b = &t->blocks[row][col];
            if (b->active) {
                int newX = t->x + col + dx;
                int newY = t->y + row + dy;

                // Check walls (assuming 0..11 width and 0..21 height)
                if (newX < 1 || newX > 11 || newY > 21) {
                    return false;
                }

                // Check collisions with placed blocks
                if (app->filledBlocks[newY][newX].v == true) {
                    return false;
                }
            }
        }
    }
    return true;
}
