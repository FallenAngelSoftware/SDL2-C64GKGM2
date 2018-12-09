#ifndef INPUT
#define INPUT

class Input
{
public:

	Input(void);
	virtual ~Input(void);

    bool EXIT_Game;

    bool ScreenIsDirty;

    bool MouseButtonsRaw[2];
    bool MouseButtonPressed[2];
    bool MouseButtonWasClicked[2];
    int MouseX, MouseY;

    SDL_Event Event;

    SDL_Keycode KeyOnKeyboardPressedByUser;
    int DelayAllUserInput;
    bool ShiftKeyPressed;
    SDL_Keymod KeyModState;

    void GetAllUserInput(void);
};

#endif

