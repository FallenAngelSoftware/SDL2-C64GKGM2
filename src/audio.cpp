#include "SDL.h"

#include <stdio.h>

#include "audio.h"

#include "visuals.h"

extern Visuals* visuals;

//-------------------------------------------------------------------------------------------------
Audio::Audio(void)
{

}

//-------------------------------------------------------------------------------------------------
Audio::~Audio(void)
{
    SDL_CloseAudioDevice(deviceId);

    for (int index = 0; index < NumberOfWAVs; index++)
    {
       SDL_FreeWAV(AudioWAVs[index].wavBuffer);
    }
}

//-------------------------------------------------------------------------------------------------
bool Audio::SetupAudio(void)
{
    if (SDL_LoadWAV("data/audio/Menu-Click.wav", &AudioWAVs[0].wavSpec, &AudioWAVs[0].wavBuffer, &AudioWAVs[0].wavLength) == NULL)
    {
        fprintf(stderr, "Could not open .wav: %s\n", SDL_GetError());
        visuals->CoreFailure = true;
        return(false);
    }

    deviceId = SDL_OpenAudioDevice(NULL, 0, &AudioWAVs[0].wavSpec, NULL, 0);

    return(true);
}

//-------------------------------------------------------------------------------------------------
void Audio::PlayAudio(int index)
{
    if (index > NumberOfWAVs)  return;

    SDL_QueueAudio(deviceId, AudioWAVs[index].wavBuffer, AudioWAVs[index].wavLength);
    SDL_PauseAudioDevice(deviceId, 0);
}
