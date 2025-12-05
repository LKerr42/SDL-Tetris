#ifndef AUDIO_H
#define AUDIO_H

#include <SDL3/SDL_audio.h>

/**
 * sound:
 * Represents a loaded audio file and its playback state.
 */
typedef struct {
    char source[32];
    Uint8 *audio_buff;
    Uint32 audio_len;
    Uint32 cursor;
    bool playing;
    SDL_AudioStream *stream;
} sound;
extern sound sfx[9];
extern sound mainTheme;

/**
 * Starts the feedback for a sound.
 * Use this function once when the sound needs to start.
 * 
 * \param *s Pointer to the sound struct instance you want to play
 */
void startSound(sound *s);

/**
 * Initalise a sound bite. 
 * Reference should be to a predeclared instance of the struct.
 * 
 * \param *wavFile Pointer to the struct instance to init
 * \param path[]   String of the path to the WAV file
 */
void initaliseAudioFile(sound *wavFile, char path[]);

/**
 * Continue to play an audio bite.
 * Run this each frame, it will feed in the correct about of buffer for the frame.
 * Only feeds in if startSound is called and playing is set to true.
 * 
 * \param *wavFile Pointer to the struct instance to play
 * \param loop     True if the audio should loop without stop
 */
void playWAV(sound *wavFile, bool loop);

/**
 * Restart the main theme.
 * Clears the stream for the main theme. Really a helper function for readability.
 */
void restartMainTheme();

#endif