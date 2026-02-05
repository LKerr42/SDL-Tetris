#include "include/stats.h"
#include "include/app.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printStats(appContext *app) {
    printf("\n--printed data--\n");
    printf("totalLinesCleared = %d\n", app->userStats->totalLinesCleared);
    printf("highestLevel = %d\n", app->userStats->highestLevel);
    printf("highestScore = %d\n", app->userStats->highestScore);
    printf("gamesPlayed = %d\n", app->userStats->gamesPlayed);
    printf("totalTetrises = %d\n", app->userStats->totalTetrises);

    printf("gameLinesCleared = %d\n", app->userStats->gameLinesCleared);
    printf("gameSpecTetsDropped = {\n");
    for (int i = 0; i < 7; i++) {
        printf("    [%d]: %d\n", i, app->userStats->gameSpecTetsDropped[i]);
    }
    printf("}\n");
}

void initUserStats(appContext *app) {
    app->userStats = calloc(1, sizeof(stats));
  
    char *basePath = (char*)SDL_GetBasePath();
    snprintf(app->filePath, sizeof(app->filePath), "%suserData\\stats.txt", basePath);
    SDL_free(basePath);

    app->dataFile = fopen(app->filePath, "r");
    if (!app->dataFile) {
        writeToStatsFile(app);
    }

    char line[128];
    while (fgets(line, sizeof(line), app->dataFile)) {
        int value;

        if (line[strlen(line)-1] == '\n') {
            line[strlen(line)-1] = '\0';
        }

        if (sscanf(line, "total_lines_cleared=%d", &value) == 1) {
            app->userStats->totalLinesCleared = value;
        } else if (sscanf(line, "highest_level=%d", &value) == 1) {
            app->userStats->highestLevel = value;
        } else if (sscanf(line, "highest_score=%d", &value) == 1) {
            app->userStats->highestScore = value;
        } else if (sscanf(line, "games_played=%d", &value) == 1) {
            app->userStats->gamesPlayed = value;
        } else if (sscanf(line, "total_tetrises=%d", &value) == 1) { 
            app->userStats->totalTetrises = value;
        }

        printf("%s\n", line);
    }

    printStats(app);

    fclose(app->dataFile);
}

void writeToStatsFile(appContext *app) {
    app->dataFile = fopen(app->filePath, "w");

    fprintf(app->dataFile, "total_lines_cleared=%d\n", app->userStats->totalLinesCleared);
    fprintf(app->dataFile, "highest_level=%d\n", app->userStats->highestLevel);
    fprintf(app->dataFile, "highest_score=%d\n", app->userStats->highestScore);
    fprintf(app->dataFile, "games_played=%d\n", app->userStats->gamesPlayed);
    fprintf(app->dataFile, "total_tetrises=%d\n", app->userStats->totalTetrises);

    fclose(app->dataFile);
}