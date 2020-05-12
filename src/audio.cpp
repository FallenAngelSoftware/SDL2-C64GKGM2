#include <stdio.h>

#include "SDL.h"

#include "audio.h"

//-------------------------------------------------------------------------------------------------
Audio::Audio(void)
{
    MusicCurrentlyPlaying = -1;

    SDL_LoadWAV("data/audio/BGM-IDE1.wav", &MusicSpec[0], &MusicBuffer[0], &MusicLength[0]);

    SDL_LoadWAV("data/audio/SFX-MenuMove.wav", &SoundEffectSpec[0], &SoundEffectBuffer[0], &SoundEffectLength[0]);
    SDL_LoadWAV("data/audio/SFX-MenuClick.wav", &SoundEffectSpec[1], &SoundEffectBuffer[1], &SoundEffectLength[1]);

}

//-------------------------------------------------------------------------------------------------
Audio::~Audio(void)
{
    SDL_CloseAudioDevice(AudioDeviceID);

    SDL_FreeWAV(MusicBuffer[0]);

    SDL_FreeWAV(SoundEffectBuffer[0]);
    SDL_FreeWAV(SoundEffectBuffer[1]);

}

//-------------------------------------------------------------------------------------------------
void Audio::PlayMusic(int musicIndex)
{
    AudioDeviceID = SDL_OpenAudioDevice(NULL, 0, &MusicSpec[musicIndex], NULL, 0);
    if (AudioDeviceID == 0)
    {
        printf( "Failed to open audio device: %s\n", SDL_GetError() );
    }

    SDL_QueueAudio(AudioDeviceID, MusicBuffer[musicIndex], MusicLength[musicIndex]);
    SDL_PauseAudioDevice(AudioDeviceID, 0);

    MusicCurrentlyPlaying = musicIndex;

}

//-------------------------------------------------------------------------------------------------
void Audio::PlaySoundEffect(int soundEffectIndex)
{
    Uint32 sizeAudio = SDL_GetQueuedAudioSize(AudioDeviceID);
    SDL_ClearQueuedAudio(AudioDeviceID);
    Uint8 * ptr = MusicBuffer[MusicCurrentlyPlaying] + MusicLength[MusicCurrentlyPlaying] - sizeAudio;
    SDL_MixAudioFormat(ptr, SoundEffectBuffer[soundEffectIndex], AUDIO_S16, SoundEffectLength[soundEffectIndex], SDL_MIX_MAXVOLUME);
    SDL_QueueAudio(AudioDeviceID, ptr, sizeAudio);
    SDL_PauseAudioDevice(AudioDeviceID, 0);
}
