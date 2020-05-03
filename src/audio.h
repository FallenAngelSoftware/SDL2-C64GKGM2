#ifndef AUDIO
#define AUDIO

class Audio
{
public:

	Audio(void);
	virtual ~Audio(void);

    #define AUDIO_FORMAT AUDIO_S16LSB
    #define AUDIO_FREQUENCY 22050
    #define AUDIO_CHANNELS 1
    #define AUDIO_SAMPLES 4096
    #define AUDIO_MAX_SOUNDS 25
    #define SDL_AUDIO_ALLOW_CHANGES SDL_AUDIO_ALLOW_ANY_CHANGE

    struct sound
    {
        uint32_t length;
        uint32_t lengthTrue;
        uint8_t * bufferTrue;
        uint8_t * buffer;
        uint8_t loop;
        uint8_t fade;
        uint8_t free;
        uint8_t volume;

        SDL_AudioSpec audio;

        struct sound * next;
    } AudioClass;

    Audio * createAudio(const char * filename, uint8_t loop, int volume);

    Audio * music;

    void freeAudio(Audio * audio);

    void playSound(const char * filename, int volume);

    void playMusic(const char * filename, int volume);

    void playSoundFromMemory(Audio * audio, int volume);

    void playMusicFromMemory(Audio * audio, int volume);

    void endAudio(void);

    bool initAudio(void);

    void pauseAudio(void);

    void unpauseAudio(void);
};

#endif
