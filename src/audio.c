#include "include/audio.h"

sound sfx[9];
sound mainTheme;

void startSound(sound *s) {
    s -> playing = true;
    SDL_ClearAudioStream(s->stream);
}

void initaliseAudioFile(sound *wavFile, char path[]) {
    SDL_AudioSpec spec;

    strcpy(wavFile -> source, path);
    wavFile -> audio_buff = NULL;
    wavFile -> audio_len = 0;
    wavFile -> playing = false;
    wavFile -> cursor = 0;

    if (!SDL_LoadWAV(path, &spec, &wavFile->audio_buff, & wavFile->audio_len)) {
        SDL_Log("Couldn't load .wav file: %s", SDL_GetError());
    }

    wavFile -> stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
    if (!wavFile -> stream) {
        SDL_Log("Couldn't create audio stream: %s", SDL_GetError());
    }
    SDL_ResumeAudioStreamDevice(wavFile -> stream);
}

void playWAV(sound *wavFile, bool loop) {
    if (!loop) {
        if (!wavFile->playing) return;

        int queued = SDL_GetAudioStreamQueued(wavFile -> stream);

        Uint32 remaining = wavFile->audio_len - wavFile->cursor;

        if (queued < 48000) {
            if (remaining > 0) {
                SDL_PutAudioStreamData(
                    wavFile -> stream,
                    wavFile->audio_buff + wavFile->cursor,
                    remaining
                );
                wavFile->cursor += remaining;
            }
        }

        if (wavFile->cursor >= wavFile->audio_len && queued == 0) {
            wavFile->playing = false;
            wavFile->cursor = 0;
        }
    } else {
        if (SDL_GetAudioStreamQueued(wavFile -> stream) < ((int) wavFile -> audio_len)) {
            SDL_PutAudioStreamData(wavFile -> stream, wavFile -> audio_buff, (int) wavFile -> audio_len);
        }
    }
}

void restartMainTheme() {
    SDL_ClearAudioStream(mainTheme.stream);
}