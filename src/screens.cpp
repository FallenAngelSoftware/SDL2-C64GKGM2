#include "SDL.h"

#include <stdio.h>
#include <cstring>

#include "screens.h"

#include "visuals.h"
#include "input.h"
#include "audio.h"
#include "interface.h"
#include "logic.h"

extern Visuals* visuals;
extern Input* input;
extern Audio* audio;
extern Interface* interface;
extern Logic* logic;

//-------------------------------------------------------------------------------------------------
Screens::Screens(void)
{
    DEBUG = true;

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

            interface->DestroyAllButtons();

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

    if (ScreenTransitionStatus != FadeNone)  ScreenIsDirty = true;

    SDL_GetWindowSize(visuals->Window, &windowWidth, &windowHeight);
    if (windowWidth != visuals->WindowWidthCurrent || windowHeight != visuals->WindowHeightCurrent)
    {
        visuals->WindowWidthCurrent = windowWidth;
        visuals->WindowHeightCurrent = windowHeight;

        ScreenIsDirty = true;
    }

    interface->ProcessAllButtons();

    switch(ScreenToDisplay)
    {
        case FAS_Screen:
            DisplayFAS_Screen();
            break;

        case SDL_Screen:
            DisplaySDL_Screen();
            break;

        case Main_Screen:
            DisplayMain_Screen();
            break;

        default:
            break;
    }

    interface->DisplayAllButtonsOnScreenBuffer();

    ApplyScreenFadeTransition();

    if (DEBUG == true)
    {
        visuals->Sprites[2].ScreenX = 320;
        visuals->Sprites[2].ScreenY = 9;
        visuals->DrawSpriteOntoScreenBuffer(2);

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

//        strcat(visuals->VariableText, " ");
//        sprintf(temp, "%d", logic->CommandScrollSpeed);
//        strcat(visuals->VariableText, temp);

        visuals->DrawSentenceOntoScreenBuffer(0, visuals->VariableText, 8, 8, JustifyLeft, 255, 255, 255, 255, 1.1, 1.1);
    }

//    if (ScreenIsDirty == true)
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

    if (ScreenIsDirty == true)
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

    if (ScreenIsDirty == true)
    {
        visuals->ClearScreenBufferWithColor(0, 0, 0, 255);

        visuals->Sprites[20].ScreenX = 320;
        visuals->Sprites[20].ScreenY = 240;
        visuals->DrawSpriteOntoScreenBuffer(20);
    }

    if (ScreenTransitionStatus == FadeOut && ScreenFadeTransparency == 255)
    {
        ScreenTransitionStatus = FadeAll;
        ScreenToDisplay = Main_Screen;
    }
}

//-------------------------------------------------------------------------------------------------
void Screens::DisplayMain_Screen(void)
{
    if (ScreenTransitionStatus == FadeAll)
    {
        int buttonScreenX = 595;
        int buttonScreenY = 40;
        int buttonOffsetY = 57;
        interface->CreateButtonWithText(true, "MENU", 1200, buttonScreenX, buttonScreenY, 255, 255, 255, 255, 0.3, 1.0);
        buttonScreenY+=buttonOffsetY;
        interface->CreateButtonWithText(true, "FILE", 1200, buttonScreenX, buttonScreenY, 255, 255, 255, 255, 0.3, 1.0);
        buttonScreenY+=buttonOffsetY;
        interface->CreateButtonWithText(true, "CLEAR", 1200, buttonScreenX, buttonScreenY, 255, 255, 255, 255, 0.3, 1.0);
        buttonScreenY+=buttonOffsetY;
        interface->CreateButtonWithText(true, "RUN", 1200, buttonScreenX, buttonScreenY, 255, 255, 255, 255, 0.3, 1.0);
        buttonScreenY+=buttonOffsetY;
        interface->CreateButtonWithText(true, "INSERT", 1200, buttonScreenX, buttonScreenY, 255, 255, 255, 255, 0.3, 1.0);
        buttonScreenY+=buttonOffsetY;
        interface->CreateButtonWithText(true, "DELETE", 1200, buttonScreenX, buttonScreenY, 255, 255, 255, 255, 0.3, 1.0);
        buttonScreenY+=buttonOffsetY;
        interface->CreateButtonWithText(true, "FIND", 1200, buttonScreenX, buttonScreenY, 255, 255, 255, 255, 0.3, 1.0);
        buttonScreenY+=buttonOffsetY;
        interface->CreateButtonWithText(true, "COPY", 1200, buttonScreenX, buttonScreenY, 255, 255, 255, 255, 0.3, 1.0);

        interface->CreateButtonWithText(false, "", 1210, 21, 132, 255, 255, 255, 255, 0.5, 1.2);
        interface->CreateButtonWithText(false, "", 1215, 21, 132+90, 255, 255, 255, 255, 0.5, 1.2);

        interface->CreateButtonWithText(false, "", 1210, 21, 132+199, 255, 255, 255, 255, 0.5, 1.2);
        interface->CreateButtonWithText(false, "", 1215, 21, 132+90+199, 255, 255, 255, 255, 0.5, 1.2);

        ScreenTransitionStatus = FadeIn;
    }

    if (ScreenIsDirty == true)
    {
        visuals->ClearScreenBufferWithColor(100, 100, 100, 255);

        visuals->Sprites[100].ScreenX = 320;
        visuals->Sprites[100].ScreenY = 240;
        visuals->DrawSpriteOntoScreenBuffer(100);

        visuals->Sprites[1250].ScreenX = 320-25;
        visuals->Sprites[1250].ScreenY = 47;
        visuals->Sprites[1250].ScaleX = 2.0;
        visuals->Sprites[1250].ScaleY = 1.4;
        visuals->DrawSpriteOntoScreenBuffer(1250);

        visuals->Sprites[1250].ScreenX = 320-25;
        visuals->Sprites[1250].ScreenY = 47+130;
        visuals->Sprites[1250].ScaleX = 2.0;
        visuals->Sprites[1250].ScaleY = 4.75;
        visuals->DrawSpriteOntoScreenBuffer(1250);



visuals->DrawSentenceOntoScreenBuffer(1, "NAME-", 410, 35, JustifyLeft, 255, 255, 255, 255, 1.0, 2.0);
visuals->DrawSentenceOntoScreenBuffer(1, "INTRO", 410+50, 35, JustifyLeft, 255, 255, 255, 255, 1.0, 2.0);

visuals->DrawSentenceOntoScreenBuffer(1, "FREE-", 410, 35+23, JustifyLeft, 255, 255, 255, 255, 1.0, 2.0);
visuals->DrawSentenceOntoScreenBuffer(1, "1999", 410+50, 35+23, JustifyLeft, 255, 255, 255, 255, 1.0, 2.0);

if (interface->ThisButtonWasPressed == 8)
{
    if (logic->CommandDisplayStartIndex > 0)
    {
        logic->CommandDisplayStartIndex--;
        logic->CommandDisplayEndIndex--;
    }
}
else if (interface->ThisButtonWasPressed == 9)
{
    if (logic->CommandDisplayStartIndex < 98)
    {
        logic->CommandDisplayStartIndex++;
        logic->CommandDisplayEndIndex++;
    }
}


int commandScreenY = 109;
int commandOffsetY = 32;
for (int index = logic->CommandDisplayStartIndex; index < logic->CommandDisplayEndIndex; index++)
{
    visuals->DrawSentenceOntoScreenBuffer(1, visuals->Commands[index].CommandTexts, 65, commandScreenY, JustifyLeft, 255, 255, 255, 255, 1.6, 2.0);
    commandScreenY+=commandOffsetY;
}



        visuals->Sprites[1251].ScreenX = 320-25;
        visuals->Sprites[1251].ScreenY = 47+130+200;
        visuals->Sprites[1251].ScaleX = 2.0;
        visuals->Sprites[1251].ScaleY = 4.75;
        visuals->DrawSpriteOntoScreenBuffer(1251);
    }

    if (ScreenTransitionStatus == FadeOut && ScreenFadeTransparency == 255)
    {
        ScreenTransitionStatus = FadeAll;
        ScreenToDisplay = SDL_Screen;
    }
}
