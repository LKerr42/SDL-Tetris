#include "include/app.h"
#include "include/tetromino.h"
#include "include/audio.h"
#include "include/stats.h"
#include <string.h>
#include <stdio.h>

void closeApp(appContext *app) {
    writeToStatsFile(app);
    
    SDL_DestroyTexture(app->boardTexture);
    SDL_DestroyTexture(app->staticText);
    SDL_DestroyTexture(app->keyboard);
    SDL_DestroyTexture(app->keyboardText);
    for (int k = 0; k < 300; k++) {
        if (app->textArray->tex != NULL) {
            SDL_DestroyTexture(app->textArray->tex);
        }
    }

    TTF_CloseFont(app->globalFont);
    TTF_CloseFont(app->globalFontS);
    TTF_Quit();

    for (int i = 0; i < arraySize(sfx); i++) {
        if (sfx[i].stream) {
            SDL_DestroyAudioStream(sfx[i].stream);
        }
        SDL_free(sfx[i].audio_buff);
    }
    SDL_DestroyAudioStream(mainTheme.stream);
    SDL_free(mainTheme.audio_buff);

    SDL_free(app->currentTet);
    for (int j = 0; j < 7; j++) {
        SDL_free(app->tetArray[j]);
        if (j < 6) SDL_free(app->titleTetroes[j]);
    }

    SDL_free(app->userStats);
    fclose(app->dataFile);

    SDL_DestroyRenderer(app->renderer);
    SDL_DestroyWindow(app->window);
}