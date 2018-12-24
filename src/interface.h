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
        float TextScaleX;
        float TextScaleY;
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
        bool BlockPressing;
        int InterfaceLevel;
        bool PlaySound;
    } Buttons[NumberOfButtons];

    int ThisButtonWasPressed;

    int CodeSelectorButtonsStart;
    int CodeSelectorButtonsEnd;
    int CodeLineSelectorButtonsStart;
    int CodeLineSelectorButtonsEnd;

    int CurrentInterfaceLevel;
    bool InterfaceLevelBackgroundShown;

    int EditorResizeButtonOriginalPressY;
    int EditorResizeButtonY;
    int EditorResizeButtonYoffset;
    int CodingWindowsValue;

    int EditQuitButtonIndex;
    int EditDelButtonIndex;
    int EditMoveButtonIndex;
    int EditCopyButtonIndex;

    #define EditStatusOff               -1
    #define EditStatusFirst             0
    #define EditStatusLast              1
    #define EditStatusCommand           2
    #define EditStatusLocationMove      3
    #define EditStatusLocationCopy      4
    int EditStatus;

    void SetupCodingWindows(void);

    int CreateButtonWithText(int level, bool oneClick, bool playSound, const char *textToDisplay, float textScaleX, float textScaleY, Uint16 spriteIndex, int screenX, int screenY, Uint8 red, Uint8 green, Uint8 blue, Uint8 transparency, float scaleX, float scaleY);
    void DisplayAllButtonsOnScreenBuffer(int level);
    void ProcessAllButtons(void);
    void DestroyAllButtons(void);

};

#endif
