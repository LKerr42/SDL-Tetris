#ifndef STATS_H
#define STATS_H

#include <stdint.h>

typedef struct appContext appContext;

typedef struct stats {
    //All time
    unsigned int highestScore;
    uint16_t totalLinesCleared;
    uint16_t highestLevel;  
    uint16_t gamesPlayed;
    uint16_t totalTetrises;

    //Game specific
    uint8_t gameSpecTetsDropped[7];
    uint16_t gameLinesCleared;
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
 * Call on app close or if the file doesn't exist.
 * 
 * \param *app  Pointer to the app context
 */
void writeToStatsFile(appContext *app);

#endif