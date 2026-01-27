#include "include/game_control.h"
#include "include/audio.h"
#include "include/app.h"
#include "include/tetromino.h"
#include "include/renderer.h"
#include "include/text.h"
#include "include/stats.h"
#include <stdio.h>

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
        app->userStats->gameSpecTetsDropped[i] = 0;
        if (i < 4) {
            app->nextBlocks[i] = SDL_rand(7);
        }
    }
    app->score = 0;
    sprintf(app->scoreString, "%s", "0000000");
    updateScoreTexture(app);

    app->heldtet = -1;
    app->currentBlock = 0;

    app->currentTet->x = 4;
    app->currentTet->y = 1;

    app->winning = true;
    app->loseCard = false;
    app->firstRun = true;
        
    app->fallSpeed = 1000;
    app->totalLinesCleared = 0;
    app->level = 0;
    app->userStats->gameLinesCleared = 0;
    updateLevelTexture(app);
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

void rotateTetrominoCCW(appContext *app, tetromino *t) {
    startSound(&sfx[SPINCCW]);
    int N, Y, X, rx, ry;
    blockStruct temp[4][4];
    if (app->currentBlock == 0) {
        N = 4;
    } else if (app->currentBlock == 3) {
        N = 2;
    } else {
        N = 3;
    }

    for (Y = 0; Y < 4; Y++) {
        for (X = 0; X < 4; X++) {
            temp[Y][X].active = false;

            temp[Y][X].x = X;
            temp[Y][X].y = Y;

            temp[Y][X].r = t->r;
            temp[Y][X].g = t->g;
            temp[Y][X].b = t->b;
        }
    }

    for (Y = 0; Y < N; Y++) { 
        for (X = 0; X < N; X++) {
            blockStruct src = t->blocks[Y][X];

            rx = Y;
            ry = 1 - (X - (N - 2));

            temp[ry][rx] = src;

            temp[ry][rx].x = rx;
            temp[ry][rx].y = ry;

            temp[ry][rx].r = t->r;
            temp[ry][rx].g = t->g;
            temp[ry][rx].b = t->b;
        }
    }

    for (Y = 0; Y < 4; Y++) {
        for (X = 0; X < 4; X++) {
            t->blocks[Y][X] = temp[Y][X];
        }
    }

    if (!canMove(app, t, 0, 0)) {
        if (canMove(app, t, 1, 0)) {
            t->x += 1;
        } else if (canMove(app, t, -1, 0)) {
            t->x -= 1;
        } else {
            rotateTetrominoCW(app, t);
        }
    }
}

void rotateTetrominoCW(appContext *app, tetromino *t) {
    startSound(&sfx[SPINCW]);
    int N, Y, X;
    blockStruct temp[4][4];
    if (app->currentBlock == 0) {
        N = 4;
    } else if (app->currentBlock == 3) {
        N = 2;
    } else {
        N = 3;
    }

    for (Y = 0; Y < 4; Y++) { //FIX: the active, x and y variables have to be updated aswell
        for (X = 0; X < 4; X++) {
            temp[Y][X].active = false;

            temp[Y][X].x = X;
            temp[Y][X].y = Y;

            temp[Y][X].r = t->r;
            temp[Y][X].g = t->g;
            temp[Y][X].b = t->b;
        }
    }

    for (Y = 0; Y < N; Y++) { 
        for (X = 0; X < N; X++) {
            blockStruct src = t->blocks[Y][X];

            int rx = 1 - (Y - (N - 2));
            int ry = X;

            temp[ry][rx] = src;

            temp[ry][rx].x = rx;
            temp[ry][rx].y = ry;

            temp[ry][rx].r = t->r;
            temp[ry][rx].g = t->g;
            temp[ry][rx].b = t->b;
        }
    }

    for (Y = 0; Y < 4; Y++) {
        for (X = 0; X < 4; X++) {
            t->blocks[Y][X] = temp[Y][X];
        }
    }


    if (!canMove(app, t, 0, 0)) {
        if (canMove(app, t, 1, 0)) {
            t->x += 1;
        } else if (canMove(app, t, -1, 0)) {
            t->x -= 1;
        } else {
            rotateTetrominoCCW(app, t);
        }
    }
}

void moveLineDown(appContext *app, int remove) {
    //start at the index and set each to the one above
    for (int i = remove; i >= 2; i--) {
        for (int j = 1; j <= 10; j++) {
            app->filledBlocks[i][j] = app->filledBlocks[i-1][j];
        }
    }
}

void moveBoardDown(appContext *app) {
    for (int i = 1; i < 5; i++) {
        if (app->clearInst.rows[i] != -1) {
            moveLineDown(app, app->clearInst.rows[i]);
        }
    }
}

void clearLinesStruct(appContext *app) {
    app->clearInst.rows[0] = 67;
    app->clearInst.amountLines = 0;
    app->clearInst.column = 1;
    app->clearInst.active = false;
    for (int i = 1; i < 5; i++) {
        app->clearInst.rows[i] = -1;
    }

}

bool pushBackToLinesArray(appContext *app, int value) {
    for (int i = 5; i >= 1; i--) {
        if (app->clearInst.rows[i-1] != -1) {
            app->clearInst.rows[i] = value;
            return true;
        } 
    }
    return false;
}

void startLineClear(appContext *app) {
    int centre = 5;
    app->clearInst.lColumn = centre-1;
    app->clearInst.rColumn = centre;

    app->clearInst.active = true;
    app->clearInst.lastStep = SDL_GetTicks();
    app->paused = true;
}

void updateLineClear(appContext *app, uint64_t now) {
    if (!app->clearInst.active || app->userPause) return;

    if (now - app->clearInst.lastStep >= 90) {
        displayLineClearColumns(app);

        app->clearInst.lColumn--;
        app->clearInst.rColumn++;
        app->clearInst.lastStep = now;

        //adding score
        /*if (app->clearInst.column < 12) {
            app->score += app->scoreTenth;
            updateScoreTexture(app);
        }*/

        if (app->clearInst.lColumn == 0 && app->clearInst.rColumn == 10) {
            if (app->totalLinesCleared / 10 > app->level) { //New level
                app->level++;
                if (app->level > app->userStats->highestLevel) {
                    app->userStats->highestLevel = app->level;
                }
                app->fallSpeed -= 50;
                updateLevelTexture(app);
            }
            if (app->score > app->userStats->highestScore) {
                app->userStats->highestScore = app->score;
            }

            app->paused = false;
            moveBoardDown(app);
            runWireframes(app, app->currentTet);
            clearLinesStruct(app);
            buildBoardTexture(app);
        }
    }
}