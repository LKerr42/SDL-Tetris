#ifndef TETROMINO_H
#define TETROMINO_H

typedef struct {
    int x;
    int y;
    int r;
    int g;
    int b;
    bool active;
} blockStruct;
blockStruct block;

typedef struct {
    blockStruct blocks[4][4];
    blockStruct titleBlocks[5][4];
    int r;
    int g;
    int b;
    int x;
    int y;
} tetromino;

#endif