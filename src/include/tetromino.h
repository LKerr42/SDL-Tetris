#ifndef TETROMINO_H
#define TETROMINO_H

#include <stdbool.h>

/**
 * blockStruct:
 * Representation of a block in a tetromino, x and y values are within the local 4x4 of the tetromino.
 */
typedef struct {
    int x;
    int y;
    int r;
    int g;
    int b;
    bool active;
} blockStruct;
blockStruct block;

/**
 * tetromino:
 * Representation of a tetromino, array of blockStructs encode critical data
 */
typedef struct {
    blockStruct blocks[4][4];
    blockStruct titleBlocks[5][4];
    int r;
    int g;
    int b;
    int x;
    int y;
} tetromino;

/**
 * colours:
 * Representation of a RGB value, implimented for ease of use of colours
 */
typedef struct {
    int r;
    int g;
    int b;
} colours;
colours colour[7];

/**
 * Set the colour of a blockStruct instance
 * 
 * \param *app  Pointer to the instance to update
 * \param R     New r RGB value
 * \param G     New g RGB value
 * \param B     New b RGB value
 */
void setBlockColour(blockStruct *block, int R, int G, int B);

/**
 * Set the colour of a tetromino instance
 * 
 * \param *app  Pointer to the instance to update
 * \param R     New r RGB value
 * \param G     New g RGB value
 * \param B     New b RGB value
 */
void setTetColour(tetromino *object, int R, int G, int B);

/**
 * Set the colour of a colours instance
 * 
 * \param *app  Pointer to the instance to update
 * \param R     New r RGB value
 * \param G     New g RGB value
 * \param B     New b RGB value
 */
void setColourDef(colours *data, int R, int G, int B);

#endif