#include "include/tetromino.h"
#include "include/app.h"
#include <stdlib.h>
#include <stdio.h>

void setBlockColour(blockStruct *block, int R, int G, int B) {
    block -> r = R;
    block -> g = G;
    block -> b = B;
}

void setTetColour(tetromino *object, int R, int G, int B) {
    object -> r = R;
    object -> g = G;
    object -> b = B;
}

void setColourDef(colours *data, int R, int G, int B) {
    data -> r = R;
    data -> g = G;
    data -> b = B;
}

void setupTetrominos(appContext *app) {
    int shapes[7][4][4] = {
        //Long boy
        {{0, 0, 0, 0}, 
         {1, 1, 1, 1},
         {0, 0, 0, 0},
         {0, 0, 0, 0}},
        //Left L
        {{1, 0, 0, 0},
         {1, 1, 1, 0},
         {0, 0, 0, 0},
         {0, 0, 0, 0}},
        //Right L
        {{0, 0, 1, 0},
         {1, 1, 1, 0},
         {0, 0, 0, 0},
         {0, 0, 0, 0}},
        //Square
        {{1, 1, 0, 0},
         {1, 1, 0, 0},
         {0, 0, 0, 0},
         {0, 0, 0, 0}},
        //Right squiggle
        {{0, 1, 1, 0},
         {1, 1, 0, 0},
         {0, 0, 0, 0},
         {0, 0, 0, 0}},
        //T boy
        {{0, 1, 0, 0},
         {1, 1, 1, 0},
         {0, 0, 0, 0},
         {0, 0, 0, 0}},
        //Left squiggle
        {{1, 1, 0, 0},
         {0, 1, 1, 0},
         {0, 0, 0, 0},
         {0, 0, 0, 0}},
    };

    printf("Allocating memory\n");
    for (int a = 0; a < 7; a++) {
        app->tetArray[a] = calloc(1, sizeof(struct tetromino));
    }

    printf("Defining colours\n");
    setTetColour(app->tetArray[0], 0, 255, 255); //Long boy
    setTetColour(app->tetArray[1], 0, 0, 255);   //Left L
    setTetColour(app->tetArray[2], 255, 160, 0); //Right L
    setTetColour(app->tetArray[3], 255, 255, 0); //Square 
    setTetColour(app->tetArray[4], 0, 255, 0);   //Right squiggle
    setTetColour(app->tetArray[5], 150, 0, 255); //T boy
    setTetColour(app->tetArray[6], 255, 0, 0);   //Left squiggle

    printf("Allocating memory for currentTet\n");
    app->currentTet = calloc(1, sizeof(struct tetromino));

    printf("Setting each tet up\n");
    for (int count = 0; count < 7; count++) {
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                app->tetArray[count]->blocks[y][x].x = x;
                app->tetArray[count]->blocks[y][x].y = y;

                if (shapes[count][y][x] == 1) {
                    app->tetArray[count]->blocks[y][x].active = true;
                    setBlockColour(
                        &app->tetArray[count]->blocks[y][x], 
                        app->tetArray[count]->r, 
                        app->tetArray[count]->g, 
                        app->tetArray[count]->b
                    );
                } else {
                    app->tetArray[count]->blocks[y][x].active = false;
                }

            }
        }
        app->tetArray[count]->x = 4;
        app->tetArray[count]->y = 1;
    }
}

void setupTitleBlocks(appContext *app) {
    int letters[6][5][4] = {
        //T
        {{1, 1, 1, 0}, 
         {0, 1, 0, 0},
         {0, 1, 0, 0},
         {0, 1, 0, 0},
         {0, 1, 0, 0}},
        //E
        {{1, 1, 1, 0}, 
         {1, 0, 0, 0},
         {1, 1, 0, 0},
         {1, 0, 0, 0},
         {1, 1, 1, 0}},
        //T
        {{1, 1, 1, 0}, 
         {0, 1, 0, 0},
         {0, 1, 0, 0},
         {0, 1, 0, 0},
         {0, 1, 0, 0}},
        //R
        {{1, 1, 1, 0}, 
         {1, 0, 0, 1},
         {1, 1, 1, 0},
         {1, 0, 1, 0},
         {1, 0, 0, 1}},
        //I
        {{1, 1, 1, 0}, 
         {0, 1, 0, 0},
         {0, 1, 0, 0},
         {0, 1, 0, 0},
         {1, 1, 1, 0}},
        //S
        {{0, 1, 1, 1}, 
         {1, 0, 0, 0},
         {0, 1, 1, 0},
         {0, 0, 0, 1},
         {1, 1, 1, 0}},
    };

    for (int count = 0; count < 6; count++) {
        app->titleTetroes[count] = calloc(1, sizeof(struct tetromino));
        setTetColour(app->titleTetroes[count], 0, 0, 255);

        for (int Y = 0; Y < 5; Y++) {
            for (int X = 0; X < 4; X++) {
                app->titleTetroes[count]->titleBlocks[Y][X].x = X;
                app->titleTetroes[count]->titleBlocks[Y][X].y = Y;

                if (letters[count][Y][X] == 1) {
                    app->titleTetroes[count]->titleBlocks[Y][X].active = true;
                    setBlockColour(
                        &app->titleTetroes[count]->titleBlocks[Y][X], 
                        app->titleTetroes[count]->r, 
                        app->titleTetroes[count]->g, 
                        app->titleTetroes[count]->b
                    );
                } else {
                    app->titleTetroes[count]->titleBlocks[Y][X].active = false;
                }
                

            }
        }
        app->titleTetroes[count]->x = (TETROMINO_BLOCK_SIZE << 2) * count;
        if (count == 4 || count == 5) app->titleTetroes[count]->x += TETROMINO_BLOCK_SIZE;
        app->titleTetroes[count]->y = 0;
    }
}
