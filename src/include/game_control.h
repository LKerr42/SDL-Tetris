#ifndef GAME_CONTROL_H
#define GAME_CONTROL_H

#include <stdbool.h>

typedef struct appContext appContext;
typedef struct tetromino tetromino;

void resetBoard(appContext *app);
void resetGame(appContext *app);
void runWireframes(appContext *app, tetromino *copyTet);
bool canMove(appContext *app, tetromino *t, int dx, int dy);

#endif