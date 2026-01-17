#ifndef STATS_H
#define STATS_H

#include <stdint.h>

typedef struct appContext appContext;

typedef struct stats {
    int totalLinesCleared;
    int highestLevel;
    int highestScore;
    int gamesPlayed;
    uint8_t specTetsDropped[7];
    int gameLinesCleared;
    int gamePiecesDropped;
} stats;

/**
 * Print the user stats to the terminal. Temp
 * 
 * \param *app  Pointer to the app context
 */
void printStats(appContext *app);

/**
 * Open the data file and read in the data. Call only once during init
 * 
 * \param *app  Pointer to the app context
 */
void initUserStats(appContext *app);

/**
 * Reopen and write the data from the struct to the data file. 
 * Call on game end, app close, or if the file doesn't exist.
 * 
 * \param *app  Pointer to the app context
 */
void writeToStatsFile(appContext *app);

#endif