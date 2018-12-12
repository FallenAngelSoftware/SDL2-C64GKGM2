#ifndef SCREENS
#define SCREENS

class Screens
{
public:

	Screens(void);
	virtual ~Screens(void);

    bool DEBUG;

    bool ScreenIsDirty;

    Uint8 ScreenFadeTransparency;
    #define FadeIn          0
    #define FadeNone        1
    #define FadeOut         2
    #define FadeAll         3
    Uint8 ScreenTransitionStatus;
    Uint32 ScreenDisplayTimer;

    #define FAS_Screen                   0
    #define SDL_Screen                   1
    #define CodeEditor_Screen            2
    int ScreenToDisplay;

    void ApplyScreenFadeTransition(void);

    void ProcessScreenToDisplay(void);
    void DisplayFAS_Screen(void);
    void DisplaySDL_Screen(void);

    void MoveSelectArrowsOffScreen(void);

    bool ThisLineNumberIsAvailable(int lineNumber);
    void ShowLineNumberSelectionDialog(void);

    void DisplayCodeEditor_Screen(void);

};

#endif


