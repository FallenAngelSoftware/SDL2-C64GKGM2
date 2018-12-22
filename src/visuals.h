#ifndef VISUALS
#define VISUALS

class Visuals
{
public:

	Visuals(void);
	virtual ~Visuals(void);

    bool CoreFailure;

    SDL_Window *Window;
    int WindowWidthCurrent;
    int WindowHeightCurrent;
    SDL_Renderer *Renderer;

    Uint32 FrameLock;
    Uint32 SystemTicks;
    Uint32 NextFrameTicks;
    Uint32 NumberOfFrames;
    Uint8 CurrentFramePerSecond;
    Uint32 FramesPerSecondArray[10];
    Uint32 NextSecondTick;
    Uint32 AverageFPS;

    #define VertJustTop             -1
    #define VertJustCentered        0
    #define VertJustBottom          1

    #define NumberOfSprites     2000
    struct Sprite
    {
        SDL_Texture *Texture;
        int ScreenX;
        int ScreenY;
        float ScaleX;
        float ScaleY;
        double RotationDegree;
        Uint8 RedHue;
        Uint8 GreenHue;
        Uint8 BlueHue;
        Uint8 Transparency;
        bool Smooth;
        bool FlipX;
        bool FlipY;
        int TextureWidthOriginal;
        int TextureHeightOriginal;
        int AnimationTimer;
        int VerticalJustification;
    } Sprites[NumberOfSprites];

    #define NumberOfFontChars   256
    struct FontChar
    {
        SDL_Texture *Texture[2];
        int ScreenX;
        int ScreenY;
        float ScaleX;
        float ScaleY;
        double RotationDegree;
        Uint8 RedHue;
        Uint8 GreenHue;
        Uint8 BlueHue;
        Uint8 Transparency;
        bool Smooth;
        bool FlipX;
        bool FlipY;
        int TextureWidthOriginal;
        int TextureHeightOriginal;
        int AnimationTimer;
    } FontChars[NumberOfFontChars];

    bool LoadSpritesAndInitialize(void);

    int FontBMPsCharWidth;
    int FontBMPsCharHeight;
    bool LoadFontBMPs(void);
    void DrawCharOntoScreenBuffer(int tileSet, Uint16 index);

    #define JustifyLeft                 0
    #define JustifyCenter               1
    #define JustifyRight                2
    #define JustifyCenterOnPoint        3
    void DrawSentenceOntoScreenBuffer(int tileSet, const char *textToDisplay, int screenX, int screenY, int justificationHorizontal, Uint8 red, Uint8 green, Uint8 blue, Uint8 transparancy, float scaleX, float scaleY);

    char VariableText[256];

    #define NumberOfCommands   110
    struct Command
    {
        const char *CommandTexts;

    } Commands[NumberOfCommands];

    void CacheAllCommandTexts(void);

    bool InitializeWindow(void);

    void CalculateFramerate(void);
    void ProcessFramerate(void);

    void ClearScreenBufferWithColor(Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha);
    void DrawSpriteOntoScreenBuffer(Uint16 index);

};

#endif
