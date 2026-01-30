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
 * Remove a row in the board and move everything else down.
 * 
 * \param *app   Pointer to the app context
 * \param remove Row indexed by array index to remove
 */
void moveLineDown(appContext *app, int remove);

/**
 * Reset the data of the main clear lines struct.
 * 
 * \param *app   Pointer to the app context
 */
void clearLinesStruct(appContext *app);

/**
 * Push back a row index value to the lines array
 * 
 * \param *app   Pointer to the app context
 * \param value  Row indexed by array index to push back
 */
bool pushBackToLinesArray(appContext *app, int value);

/**
 * Start the animation for the line clearing. Sets the active bool to true and pauses the game
 * 
 * \param *app     Pointer to the app context
 * \param centre   Centre to clear the line from
 */
void startLineClear(appContext *app, int centre);

/**
 * Start the animation for the line clearing. Sets the active bool to true and pauses the game
 * 
 * \param *app   Pointer to the app context
 * \param now    Ticks since launch in unsigned int 64 bits
 */
void updateLineClear(appContext *app, uint64_t now);

/**
 * Calculate the centre to clear a line from. Just for readability really
 * 
 * \param *app       Pointer to the app context
 */
int calculateCentreForLineClear(appContext *app);

/**
 * Calculate the portion of score to add each line clear iteration
 * 
 * \param *app    Pointer to the app context
 * \param centre  Centre value calculated by calculateCentreForLineClear
 */
void calculateScorePortion(appContext *app, int centre);

#endif