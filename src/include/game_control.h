#ifndef GAME_CONTROL_H
#define GAME_CONTROL_H

#include <stdbool.h>
#include <stdint.h>

typedef struct appContext appContext;
typedef struct tetromino tetromino;

/**
 * Reset the board array. Empties and content and resets the borders
 * 
 * \param *app  Pointer to the app context
 */
void resetBoard(appContext *app);

/**
 * Reset the full game. Resets control variables and calls resetBoard()
 * 
 * \param *app  Pointer to the app context
 */
void resetGame(appContext *app);

/**
 * Calculate the wireframes for the falling teromino. Just sends it down and rebuilds the board texture.
 * 
 * \param *app     Pointer to the app context
 * \param *copyTet Tetromino to run wireframes on
 */
void runWireframes(appContext *app, tetromino *copyTet);

/**
 * Calculate whether a tetromino can move a certian amount from where it currently is.
 * 
 * \param *app     Pointer to the app context
 * \param *copyTet Tetromino to check movement
 * \param dx       Amount to move on the x-axis
 * \param dy       Amount to move on the y-axis
 */
bool canMove(appContext *app, tetromino *t, int dx, int dy);

/**
 * Rotate a tetromino clockwise. updates the exact tetromino instance (arrays and data) so be careful
 * 
 * \param *app Pointer to the app context
 * \param *t   Tetromino to rotate
 */
void rotateTetrominoCW(appContext *app, tetromino *t);

/**
 * Rotate a tetromino counter-clockwise. updates the exact tetromino instance (arrays and data) so be careful
 * 
 * \param *app Pointer to the app context
 * \param *t   Tetromino to rotate
 */
void rotateTetrominoCCW(appContext *app, tetromino *t);

/**
 * Remove a column in the board and move everything else down.
 * 
 * \param *app   Pointer to the app context
 * \param remove Column indexed by array index to remove
 */
void moveBoardDown(appContext *app, int remove);

void clearLinesStruct(appContext *app);
bool pushBackToLinesArray(appContext *app, int value);
void startLineClear(appContext *app);
void updateLineClear(appContext *app, uint64_t now);

#endif