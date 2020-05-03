#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>

#include "audio.h"

//-------------------------------------------------------------------------------------------------
Audio::Audio(void)
{
//    Audio * music = createAudio("data/audio/BGM-IDE1.wav", 1, SDL_MIX_MAXVOLUME);

}

//-------------------------------------------------------------------------------------------------
Audio::~Audio(void)
{
    freeAudio(music);

}

//-------------------------------------------------------------------------------------------------
typedef struct privateAudioDevice
{
    SDL_AudioDeviceID device;
    SDL_AudioSpec want;
    uint8_t audioEnabled;
} PrivateAudioDevice;

static PrivateAudioDevice * gDevice;
static uint32_t gSoundCount;

static void addMusic(Audio * root, Audio * newFix);

static inline void playAudio(const char * filename, Audio * audio, uint8_t loop, int volume);

static void addAudio(Audio * root, Audio * newFix);

static inline void audioCallback(void * userdata, uint8_t * stream, int len);

//-------------------------------------------------------------------------------------------------
void Audio::playSound(const char * filename, int volume)
{
    playAudio(filename, NULL, 0, volume);
}

//-------------------------------------------------------------------------------------------------
void Audio::playMusic(const char * filename, int volume)
{
    playAudio(filename, NULL, 1, volume);
}

//-------------------------------------------------------------------------------------------------
void Audio::playSoundFromMemory(Audio * audio, int volume)
{
    playAudio(NULL, audio, 0, volume);
}

//-------------------------------------------------------------------------------------------------
void Audio::playMusicFromMemory(Audio * audio, int volume)
{
    playAudio(NULL, audio, 1, volume);
}

//-------------------------------------------------------------------------------------------------
bool Audio::initAudio(void)
{
    Audio * global;
    gDevice = (PrivateAudioDevice*)calloc(1, sizeof(PrivateAudioDevice));
    gSoundCount = 0;

    if(gDevice == NULL)
    {
        fprintf(stderr, "[%s: %d]Fatal Error: Memory c-allocation error\n", __FILE__, __LINE__);
        return(false);
    }

    gDevice->audioEnabled = 0;

    if(!(SDL_WasInit(SDL_INIT_AUDIO) & SDL_INIT_AUDIO))
    {
        fprintf(stderr, "[%s: %d]Error: SDL_INIT_AUDIO not initialized\n", __FILE__, __LINE__);
        return(false);
    }

    SDL_memset(&(gDevice->want), 0, sizeof(gDevice->want));

    (gDevice->want).freq = AUDIO_FREQUENCY;
    (gDevice->want).format = AUDIO_FORMAT;
    (gDevice->want).channels = AUDIO_CHANNELS;
    (gDevice->want).samples = AUDIO_SAMPLES;
    (gDevice->want).callback = audioCallback;
    (gDevice->want).userdata = calloc(1, sizeof(Audio));

    global = (Audio*)(gDevice->want).userdata;

    if(global == NULL)
    {
        fprintf(stderr, "[%s: %d]Error: Memory allocation error\n", __FILE__, __LINE__);
        return(false);
    }

    global->AudioClass.buffer = NULL;
    global->AudioClass.next = NULL;

    if((gDevice->device = SDL_OpenAudioDevice(NULL, 0, &(gDevice->want), NULL, SDL_AUDIO_ALLOW_CHANGES)) == 0)
    {
        fprintf(stderr, "[%s: %d]Warning: failed to open audio device: %s\n", __FILE__, __LINE__, SDL_GetError());
    }
    else
    {
        gDevice->audioEnabled = 1;

        unpauseAudio();
    }

    return(true);
}


//-------------------------------------------------------------------------------------------------
void Audio::endAudio(void)
{
    if(gDevice->audioEnabled)
    {
        pauseAudio();

        freeAudio((Audio *) (gDevice->want).userdata);

        SDL_CloseAudioDevice(gDevice->device);
    }

    free(gDevice);
}

//-------------------------------------------------------------------------------------------------
void Audio::pauseAudio(void)
{
    if(gDevice->audioEnabled)
    {
        SDL_PauseAudioDevice(gDevice->device, 1);
    }
}

//-------------------------------------------------------------------------------------------------
void Audio::unpauseAudio(void)
{
    if(gDevice->audioEnabled)
    {
        SDL_PauseAudioDevice(gDevice->device, 0);
    }
}

//-------------------------------------------------------------------------------------------------
void Audio::freeAudio(Audio * audio)
{
    Audio * temp;

    while(audio != NULL)
    {
        if(audio->AudioClass.free == 1)
        {
            SDL_FreeWAV(audio->AudioClass.bufferTrue);
        }

        temp = audio;
        audio = (Audio*)audio->AudioClass.next;

        free(temp);
    }
}

//-------------------------------------------------------------------------------------------------
Audio * Audio::createAudio(const char * filename, uint8_t loop, int volume)
{
    Audio * newFix = (Audio*)calloc(1, sizeof(Audio));

    if(newFix == NULL)
    {
        fprintf(stderr, "[%s: %d]Error: Memory allocation error\n", __FILE__, __LINE__);
        return NULL;
    }

    if(filename == NULL)
    {
        fprintf(stderr, "[%s: %d]Warning: filename NULL: %s\n", __FILE__, __LINE__, filename);
        return NULL;
    }

    newFix->AudioClass.next = NULL;
    newFix->AudioClass.loop = loop;
    newFix->AudioClass.fade = 0;
    newFix->AudioClass.free = 1;
    newFix->AudioClass.volume = volume;

    if(SDL_LoadWAV(filename, &(newFix->AudioClass.audio), &(newFix->AudioClass.bufferTrue), &(newFix->AudioClass.lengthTrue)) == NULL)
    {
        fprintf(stderr, "[%s: %d]Warning: failed to open wave file: %s error: %s\n", __FILE__, __LINE__, filename, SDL_GetError());
        free(newFix);
        return NULL;
    }

    newFix->AudioClass.buffer = newFix->AudioClass.bufferTrue;
    newFix->AudioClass.length = newFix->AudioClass.lengthTrue;
    (newFix->AudioClass.audio).callback = NULL;
    (newFix->AudioClass.audio).userdata = NULL;

    return newFix;
}

//-------------------------------------------------------------------------------------------------
static inline void playAudio(const char * filename, Audio * audio, uint8_t loop, int volume)
{
    Audio * newFix;

    if(!gDevice->audioEnabled)
    {
        return;
    }

    if(loop == 0)
    {
        if(gSoundCount >= AUDIO_MAX_SOUNDS)
        {
            return;
        }
        else
        {
            gSoundCount++;
        }
    }

    if(filename != NULL)
    {
        newFix = audio->createAudio(filename, loop, volume);
    }
    else if(audio != NULL)
    {
        newFix = (Audio*)malloc(sizeof(Audio));

        if(newFix == NULL)
        {
            fprintf(stderr, "[%s: %d]Fatal Error: Memory allocation error\n", __FILE__, __LINE__);
            return;
        }

        memcpy( (char*)newFix, (char*)audio, sizeof(Audio) );

        newFix->AudioClass.volume = volume;
        newFix->AudioClass.loop = loop;
        newFix->AudioClass.free = 0;
    }
    else
    {
        fprintf(stderr, "[%s: %d]Warning: filename and Audio parameters NULL\n", __FILE__, __LINE__);
        return;
    }

    SDL_LockAudioDevice(gDevice->device);

    if(loop == 1)
    {
        addMusic((Audio *) (gDevice->want).userdata, newFix);
    }
    else
    {
        addAudio((Audio *) (gDevice->want).userdata, newFix);
    }

    SDL_UnlockAudioDevice(gDevice->device);

}

//-------------------------------------------------------------------------------------------------
static void addMusic(Audio * root, Audio * newFix)
{
    uint8_t musicFound = 0;
    Audio * rootNext = (Audio*)root->AudioClass.next;

    while(rootNext != NULL)
    {
        if(rootNext->AudioClass.loop == 1 && rootNext->AudioClass.fade == 0)
        {
            if(musicFound)
            {
                rootNext->AudioClass.length = 0;
                rootNext->AudioClass.volume = 0;
            }

            rootNext->AudioClass.fade = 1;
        }
        else if(rootNext->AudioClass.loop == 1 && rootNext->AudioClass.fade == 1)
        {
            musicFound = 1;
        }

        rootNext = (Audio*)rootNext->AudioClass.next;
    }

    addAudio(root, newFix);
}

//-------------------------------------------------------------------------------------------------
static inline void audioCallback(void * userdata, uint8_t * stream, int len)
{
    Audio * audio = (Audio *) userdata;
    Audio * previous = audio;
    int tempLength;
    uint8_t music = 0;

    SDL_memset(stream, 0, len);

    audio = (Audio*)audio->AudioClass.next;

    while(audio != NULL)
    {
        if(audio->AudioClass.length > 0)
        {
            if(audio->AudioClass.fade == 1 && audio->AudioClass.loop == 1)
            {
                music = 1;

                if(audio->AudioClass.volume > 0)
                {
                    audio->AudioClass.volume--;
                }
                else
                {
                    audio->AudioClass.length = 0;
                }
            }

            if(music && audio->AudioClass.loop == 1 && audio->AudioClass.fade == 0)
            {
                tempLength = 0;
            }
            else
            {
                tempLength = ((uint32_t) len > audio->AudioClass.length) ? audio->AudioClass.length : (uint32_t) len;
            }

            SDL_MixAudioFormat(stream, audio->AudioClass.buffer, AUDIO_FORMAT, tempLength, audio->AudioClass.volume);

            audio->AudioClass.buffer += tempLength;
            audio->AudioClass.length -= tempLength;

            previous = audio;
            audio = (Audio*)audio->AudioClass.next;
        }
        else if(audio->AudioClass.loop == 1 && audio->AudioClass.fade == 0)
        {
            audio->AudioClass.buffer = audio->AudioClass.bufferTrue;
            audio->AudioClass.length = audio->AudioClass.lengthTrue;
        }
        else
        {
            previous->AudioClass.next = audio->AudioClass.next;

            if(audio->AudioClass.loop == 0)
            {
                gSoundCount--;
            }

            audio->AudioClass.next = NULL;
            audio->freeAudio(audio);

            audio = (Audio*)previous->AudioClass.next;
        }
    }
}

//-------------------------------------------------------------------------------------------------
static void addAudio(Audio * root, Audio * newFix)
{
    if(root == NULL)
    {
        return;
    }

    while(root->AudioClass.next != NULL)
    {
        root = (Audio*)root->AudioClass.next;
    }

    root->AudioClass.next = newFix->AudioClass.next;
}
