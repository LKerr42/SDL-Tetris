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
    printf("gameLinesCleared = %d\n", app->userStats->gameLinesCleared);
    printf("gamePiecesDropped = %d\n", app->userStats->gamePiecesDropped);
    printf("specTetsDropped = {\n");
    for (int i = 0; i < 7; i++) {
        printf("    [%d]: %d\n", i, app->userStats->specTetsDropped[i]);
    }
    printf("}\n");
}

void initUserStats(appContext *app) {
    app->userStats = calloc(1, sizeof(stats));
  
    char *basePath = SDL_GetBasePath();
    snprintf(app->filePath, sizeof(app->filePath), "%suserData\\stats.txt", basePath);
    SDL_free(basePath);
    printf("%s\n", app->filePath);

    app->dataFile = fopen(app->filePath, "r");
    if (!app->dataFile) {
        printf("error: unable to open %s\n", app->filePath);
        writeToStatsFile(app);
    }

    char line[128];
    while (fgets(line, sizeof(line), app->dataFile)) {
        int value;

        if (line[strlen(line)-1] == '\n') {
            line[strlen(line)-1] = '\0';
        }

        if (sscanf(line, "total_lines_cleared=%d", &value) == 1) {
            printf("%d -- ", value);
            app->userStats->totalLinesCleared = value;
        } else if (sscanf(line, "highest_level=%d", &value) == 1) {
            printf("%d -- ", value);
            app->userStats->highestLevel = value;
        } else if (sscanf(line, "highest_score=%d", &value) == 1) {
            printf("%d -- ", value);
            app->userStats->highestScore = value;
        } else if (sscanf(line, "games_played=%d", &value) == 1) {
            printf("%d -- ", value);
            app->userStats->gamesPlayed = value;
        } else if (strncmp(line, "spec_tets_dropped=", 18) == 0) {
            char *values = strchr(line, '=');
            if (values) {
                values++; //move past '='

                for (int i = 0; i < 7; i++) {
                    app->userStats->specTetsDropped[i] = atoi(values);

                    values = strchr(line, ','); 
                    if (!values) break;
                    values++; //move past ',' 
                }
            }
        } else {
            printf("Stupid -> ");
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
    fprintf(app->dataFile, "spec_tets_dropped=");
    for (int i = 0; i < 7; i++) {
        fprintf(app->dataFile, "%d", app->userStats->specTetsDropped[i]);
        if (i < 6) {
            fprintf(app->dataFile, ",");
        }
    }

    fclose(app->dataFile);
}