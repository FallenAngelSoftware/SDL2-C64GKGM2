#ifndef AUDIO
#define AUDIO

class Audio
{
public:

	Audio(void);
	virtual ~Audio(void);

    SDL_AudioDeviceID AudioDeviceID;

    int MusicCurrentlyPlaying;

    SDL_AudioSpec MusicSpec[1];
    Uint32 MusicLength[1];
    Uint8 *MusicBuffer[1];

    SDL_AudioSpec SoundEffectSpec[2];
    Uint32 SoundEffectLength[2];
    Uint8 *SoundEffectBuffer[2];

    void PlayMusic(int musicIndex);
    void PlaySoundEffect(int soundEffectIndex);

};

#endif

