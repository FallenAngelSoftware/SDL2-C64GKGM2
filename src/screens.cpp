#include <stdio.h>
#include <cstring>

#include "SDL.h"

#include "screens.h"

#include "visuals.h"
#include "input.h"
#include "audio.h"

extern Visuals* visuals;
extern Input* input;
extern Audio* audio;

//-------------------------------------------------------------------------------------------------
Screens::Screens(void)
{
    ScreenIsDirty = true;
    ScreenToDisplay = FAS_Screen;

    ScreenFadeTransparency = 255;
    ScreenTransitionStatus = FadeAll;

}

//-------------------------------------------------------------------------------------------------
Screens::~Screens(void)
{

}

//-------------------------------------------------------------------------------------------------
void Screens::ApplyScreenFadeTransition(void)
{
    if (ScreenTransitionStatus == FadeIn)
    {
        if ( ScreenFadeTransparency > (255/3) )
        {
            ScreenFadeTransparency -= (255/3);
            ScreenIsDirty = true;
        }
        else
        {
            ScreenFadeTransparency = 0;
            ScreenIsDirty = true;
            ScreenTransitionStatus = FadeNone;
        }
    }
    else if (ScreenTransitionStatus == FadeOut)
    {
        if ( ScreenFadeTransparency < (255/3) )
        {
            ScreenFadeTransparency += (255/3);
            ScreenIsDirty = true;
        }
        else
        {
            ScreenFadeTransparency = 255;
            ScreenIsDirty = true;
        }
    }

    if (ScreenTransitionStatus != FadeNone)
    {
        visuals->Sprites[1].ScreenX = 320;
        visuals->Sprites[1].ScreenY = 240;
        visuals->Sprites[1].Transparency = ScreenFadeTransparency;
        visuals->DrawSpriteOntoScreenBuffer(1);
    }
}

//-------------------------------------------------------------------------------------------------
void Screens::ProcessScreenToDisplay(void)
{
int windowWidth;
int windowHeight;

ScreenIsDirty = true;

    SDL_GetWindowSize(visuals->Window, &windowWidth, &windowHeight);
    if (windowWidth != visuals->WindowWidthCurrent || windowHeight != visuals->WindowHeightCurrent)
    {
        visuals->WindowWidthCurrent = windowWidth;
        visuals->WindowHeightCurrent = windowHeight;

        ScreenIsDirty = true;
    }

    switch(ScreenToDisplay)
    {
        case FAS_Screen:
            DisplayFAS_Screen();
            break;

        case SDL_Screen:
            DisplaySDL_Screen();
            break;

        default:
            break;
    }

    ApplyScreenFadeTransition();

    char temp[256];
    strcpy(visuals->VariableText, "(");
    sprintf(temp, "%i", input->MouseX);
    strcat(visuals->VariableText, temp);
    strcat(visuals->VariableText, ",");
    sprintf(temp, "%i", input->MouseY);
    strcat(visuals->VariableText, temp);
    strcat(visuals->VariableText, ") FPS=");
    sprintf(temp, "%d", visuals->AverageFPS);
    strcat(visuals->VariableText, temp);
    strcat(visuals->VariableText, "/60");
    visuals->DrawSentenceOntoScreenBuffer(visuals->VariableText, 8, 8, JustifyLeft, 255, 255, 255, 255, 1.1, 1.1);

    if (ScreenIsDirty == true)
    {
        SDL_RenderPresent(visuals->Renderer);
        ScreenIsDirty = false;
    }
 }

//-------------------------------------------------------------------------------------------------
void Screens::DisplayFAS_Screen(void)
{
    if (ScreenTransitionStatus == FadeAll)
    {
        ScreenDisplayTimer = 370;
        ScreenTransitionStatus = FadeIn;
    }

    if (input->MouseButtonPressed[0] == true
       || input->KeyOnKeyboardPressedByUser == SDLK_SPACE
       || input->KeyOnKeyboardPressedByUser == SDLK_RETURN)
    {
        ScreenDisplayTimer = 0;
        input->DelayAllUserInput = 20;
        audio->PlayAudio(0);
    }

    if (ScreenDisplayTimer > 0)  ScreenDisplayTimer--;
    else if (ScreenTransitionStatus != FadeIn)  ScreenTransitionStatus = FadeOut;

//    if (ScreenIsDirty == true)
    {
        visuals->ClearScreenBufferWithColor(0, 0, 0, 255);

        visuals->Sprites[10].ScreenX = 320;
        visuals->Sprites[10].ScreenY = 240;
        visuals->DrawSpriteOntoScreenBuffer(10);
    }

    if (ScreenTransitionStatus == FadeOut && ScreenFadeTransparency == 255)
    {
        ScreenTransitionStatus = FadeAll;
        ScreenToDisplay = SDL_Screen;
    }
}

//-------------------------------------------------------------------------------------------------
void Screens::DisplaySDL_Screen(void)
{
    if (ScreenTransitionStatus == FadeAll)
    {
        ScreenDisplayTimer = 370;
        ScreenTransitionStatus = FadeIn;
    }

    if (input->MouseButtonPressed[0] == true
       || input->KeyOnKeyboardPressedByUser == SDLK_SPACE
       || input->KeyOnKeyboardPressedByUser == SDLK_RETURN)
    {
        ScreenDisplayTimer = 0;
        input->DelayAllUserInput = 20;
        audio->PlayAudio(0);
    }

    if (ScreenDisplayTimer > 0)  ScreenDisplayTimer--;
    else if (ScreenTransitionStatus != FadeIn)  ScreenTransitionStatus = FadeOut;

//    if (ScreenIsDirty == true)
    {
        visuals->ClearScreenBufferWithColor(0, 0, 0, 255);

        visuals->Sprites[20].ScreenX = 320;
        visuals->Sprites[20].ScreenY = 240;
        visuals->DrawSpriteOntoScreenBuffer(20);
    }

    if (ScreenTransitionStatus == FadeOut && ScreenFadeTransparency == 255)
    {
        ScreenTransitionStatus = FadeAll;
        ScreenToDisplay = FAS_Screen;
    }
}
