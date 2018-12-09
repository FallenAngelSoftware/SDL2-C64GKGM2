#ifndef INTERFACE
#define INTERFACE

//            TM
// "Neo's Kiss"

class Interface
{
public:

	Interface(void);
	virtual ~Interface(void);

    #define NumberOfButtons     100
    struct Button
    {
        bool OneClick;
        int SpriteIndex;
        const char *Text;
        Sint8 ScreenIndex;
        int ScreenX;
        int ScreenY;
        Uint8 RedHue;
        Uint8 GreenHue;
        Uint8 BlueHue;
        Uint8 Transparency;
        float ScaleX;
        float ScaleY;
        float AnimationScale;
        Sint16 AnimationTimer;
    } Buttons[NumberOfButtons];

    int ThisButtonWasPressed;

    void CreateButtonWithText(bool oneClick, const char *textToDisplay, Uint16 spriteIndex, int screenX, int screenY, Uint8 red, Uint8 green, Uint8 blue, Uint8 transparency, float scaleX, float scaleY);
    void DisplayAllButtonsOnScreenBuffer(void);
    void ProcessAllButtons(void);
    void DestroyAllButtons(void);

};

#endif
