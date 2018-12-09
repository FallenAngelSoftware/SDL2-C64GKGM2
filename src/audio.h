#ifndef AUDIO
#define AUDIO

class Audio
{
public:

	Audio(void);
	virtual ~Audio(void);

    #define NumberOfWAVs        1
    struct AudioWAV
    {
        SDL_AudioSpec wavSpec;
        Uint32 wavLength;
        Uint8 *wavBuffer;
    } AudioWAVs[NumberOfWAVs];

    SDL_AudioDeviceID deviceId;

    bool SetupAudio(void);
    void PlayAudio(int index);

};

#endif
