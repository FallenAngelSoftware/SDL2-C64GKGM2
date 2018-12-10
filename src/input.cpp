#include "SDL.h"

#include "input.h"

#include "screens.h"
#include "visuals.h"
#include "logic.h"

extern Screens* screens;
extern Visuals* visuals;
extern Logic* logic;

//-------------------------------------------------------------------------------------------------
Input::Input(void)
{
    EXIT_Game = false;
    DelayAllUserInput = 0;

}

//-------------------------------------------------------------------------------------------------
Input::~Input(void)
{

}

//-------------------------------------------------------------------------------------------------
void Input::GetAllUserInput(void)
{
    KeyOnKeyboardPressedByUser = -1;

    MouseButtonsRaw[0] = false;
    MouseButtonsRaw[1] = false;

    MouseButtonPressed[0] = false;
    MouseButtonPressed[1] = false;

    if (DelayAllUserInput > 0)
    {
        DelayAllUserInput--;
        return;
    }

    KeyModState = SDL_GetModState();
    if ( (KeyModState & KMOD_LSHIFT) || (KeyModState & KMOD_RSHIFT) )  ShiftKeyPressed = true;
    else  ShiftKeyPressed = false;

    while ( SDL_PollEvent(&Event) )
    {
        switch (Event.type)
        {
            case SDL_QUIT:
                EXIT_Game = true;
                break;

            case SDL_WINDOWEVENT:
                if (Event.window.event == SDL_WINDOWEVENT_RESIZED || Event.window.event == SDL_WINDOWEVENT_SHOWN)
                {
                    screens->ScreenIsDirty = true;
                }
                break;

            case SDL_KEYDOWN:
                KeyOnKeyboardPressedByUser = SDL_GetKeyFromScancode(Event.key.keysym.scancode);

                if (KeyOnKeyboardPressedByUser == SDLK_LSHIFT || KeyOnKeyboardPressedByUser == SDLK_RSHIFT)
                    KeyOnKeyboardPressedByUser = -1;
                break;

            default:
                break;
        }
    }

//------------------------------------------------------------------------
    SDL_PumpEvents();
    SDL_GetMouseState(&MouseX, &MouseY);

    float tempX, tempXtwo;
    float tempY, tempYtwo;;
    if (visuals->WindowWidthCurrent != 640)
    {
        tempX = visuals->WindowWidthCurrent;
        tempXtwo = tempX / 640;
        tempX = MouseX;
        MouseX = (int)(tempX / tempXtwo);
    }

    if (visuals->WindowHeightCurrent != 480)
    {
        tempY = visuals->WindowHeightCurrent;
        tempYtwo = tempY / 480;
        tempY = MouseY;
        MouseY = (int)(tempY / tempYtwo);
    }

    for (int index = 0; index < 2; index++)
    {
        MouseButtonsRaw[index] = false;

        if ( SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(index+1) )
        {
            MouseButtonsRaw[index] = true;

            if (MouseButtonWasClicked[index] == false)
            {
                MouseButtonPressed[index] = true;
                MouseButtonWasClicked[index] = true;
            }
        }
        else  MouseButtonWasClicked[index] = false;
    }

    if (MouseButtonsRaw[0] == false)
    {
        logic->CommandScrollNumberMoved = 0;
        logic->CommandScrollSpeed = 3;
    }
//------------------------------------------------------------------------

}
