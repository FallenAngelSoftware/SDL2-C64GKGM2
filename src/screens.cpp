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

    if (logic->CommandSelectedByMouseOld != logic->CommandSelectedByMouse)  ScreenIsDirty = true;

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

        case CodeEditor_Screen:
            DisplayCodeEditor_Screen();
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
//        sprintf(temp, "%d", logic->CodeSelectedForEdit);
//        strcat(visuals->VariableText, temp);

        visuals->DrawSentenceOntoScreenBuffer(0, visuals->VariableText, 8, 8, JustifyLeft, 255, 255, 255, 255, 1.1, 1.1);
    }

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
        ScreenToDisplay = CodeEditor_Screen;
    }
}

//-------------------------------------------------------------------------------------------------
void Screens::DisplayCodeEditor_Screen(void)
{
    if (ScreenTransitionStatus == FadeAll)
    {
        int buttonScreenX = 595+4;
        int buttonScreenY = 40;
        int buttonOffsetY = 57;
        interface->CreateButtonWithText(true, "MENU", 1200, buttonScreenX, buttonScreenY, 255, 255, 255, 255, 0.3, 1.0);
        buttonScreenY+=buttonOffsetY;
        interface->CreateButtonWithText(true, "FILE", 1200, buttonScreenX, buttonScreenY, 255, 255, 255, 255, 0.3, 1.0);
        buttonScreenY+=buttonOffsetY;
        interface->CreateButtonWithText(true, "CLR", 1200, buttonScreenX, buttonScreenY, 255, 255, 255, 255, 0.3, 1.0);
        buttonScreenY+=buttonOffsetY;
        interface->CreateButtonWithText(true, "RUN", 1200, buttonScreenX, buttonScreenY, 255, 255, 255, 255, 0.3, 1.0);
        buttonScreenY+=buttonOffsetY;
        interface->CreateButtonWithText(true, "INS", 1200, buttonScreenX, buttonScreenY, 255, 255, 255, 255, 0.3, 1.0);
        interface->Buttons[4].BlockPressing = true;
        buttonScreenY+=buttonOffsetY;
        interface->CreateButtonWithText(true, "DEL", 1200, buttonScreenX, buttonScreenY, 255, 255, 255, 255, 0.3, 1.0);
        interface->Buttons[5].BlockPressing = true;
        buttonScreenY+=buttonOffsetY;
        interface->CreateButtonWithText(true, "FIND", 1200, buttonScreenX, buttonScreenY, 255, 255, 255, 255, 0.3, 1.0);
        buttonScreenY+=buttonOffsetY;
        interface->CreateButtonWithText(true, "COPY", 1200, buttonScreenX, buttonScreenY, 255, 255, 255, 255, 0.3, 1.0);

        interface->CreateButtonWithText(false, "", 1210, 16, 132, 255, 255, 255, 255, 0.35, 1.2);
        interface->CreateButtonWithText(false, "", 1215, 16, 132+90, 255, 255, 255, 255, 0.35, 1.2);

        interface->CreateButtonWithText(false, "", 1211, 16, 132+199, 255, 255, 255, 255, 0.35, 1.2);
        interface->CreateButtonWithText(false, "", 1216, 16, 132+90+199, 255, 255, 255, 255, 0.35, 1.2);

        logic->ScrollNumberMoved = 0;
        logic->ScrollSpeed = 3;

        logic->CommandDisplayStartIndex = 0;
        logic->CommandDisplayEndIndex = 5;

        logic->CodeSelectedByMouseOld = -1;
        logic->CodeSelectedByMouse = -1;

        logic->CodeDisplayStartIndex = 0;
        logic->CodeDisplayEndIndex = 5;

        logic->CodeSelectedByMouseOld = -1;
        logic->CodeSelectedByMouse = -1;

        logic->CodeSelectedForEdit = -1;

        int codeEditScreenY = 109+200;
        int codeEditOffsetY = 32;
        interface->CreateButtonWithText(true, "", 1220, 43, codeEditScreenY, 255, 255, 255, 255, 0.5, 1.0);
        codeEditScreenY+=codeEditOffsetY;
        interface->CreateButtonWithText(true, "", 1220, 43, codeEditScreenY, 255, 255, 255, 255, 0.5, 1.0);
        codeEditScreenY+=codeEditOffsetY;
        interface->CreateButtonWithText(true, "", 1220, 43, codeEditScreenY, 255, 255, 255, 255, 0.5, 1.0);
        codeEditScreenY+=codeEditOffsetY;
        interface->CreateButtonWithText(true, "", 1220, 43, codeEditScreenY, 255, 255, 255, 255, 0.5, 1.0);
        codeEditScreenY+=codeEditOffsetY;
        interface->CreateButtonWithText(true, "", 1220, 43, codeEditScreenY, 255, 255, 255, 255, 0.5, 1.0);

        ScreenTransitionStatus = FadeIn;
    }

    logic->RunCodeEditor();

    if (ScreenIsDirty == true)
    {
        visuals->ClearScreenBufferWithColor(100, 100, 100, 255);

        visuals->Sprites[100].ScreenX = 320;
        visuals->Sprites[100].ScreenY = 240;
        visuals->DrawSpriteOntoScreenBuffer(100);

        visuals->Sprites[1250].ScreenX = 320-26;
        visuals->Sprites[1250].ScreenY = 47;
        visuals->Sprites[1250].ScaleX = 2.1;
        visuals->Sprites[1250].ScaleY = 1.4;
        visuals->DrawSpriteOntoScreenBuffer(1250);

        visuals->Sprites[1250].ScreenX = 320-26;
        visuals->Sprites[1250].ScreenY = 47+130;
        visuals->Sprites[1250].ScaleX = 2.1;
        visuals->Sprites[1250].ScaleY = 4.75;
        visuals->DrawSpriteOntoScreenBuffer(1250);

visuals->DrawSentenceOntoScreenBuffer(1, "NAME-", 435, 35, JustifyLeft, 255, 255, 255, 255, 1.0, 2.0);
visuals->DrawSentenceOntoScreenBuffer(1, "INTRO", 640-95, 35, JustifyRight, 255, 255, 255, 255, 1.0, 2.0);

visuals->DrawSentenceOntoScreenBuffer(1, "FREE-", 435, 35+23, JustifyLeft, 255, 255, 255, 255, 1.0, 2.0);
visuals->DrawSentenceOntoScreenBuffer(1, "1999", 640-95, 35+23, JustifyRight, 255, 255, 255, 255, 1.0, 2.0);

int commandScreenY = 109;
int commandOffsetY = 32;
for (int index = logic->CommandDisplayStartIndex; index < logic->CommandDisplayEndIndex; index++)
{
    if (logic->CommandSelectedByMouse == index)
    {
        visuals->DrawSentenceOntoScreenBuffer(1, visuals->Commands[index].CommandTexts, 65, commandScreenY, JustifyLeft, 0, 255, 0, 255, 1.6, 2.0);
    }
    else
    {
        visuals->DrawSentenceOntoScreenBuffer(1, visuals->Commands[index].CommandTexts, 65, commandScreenY, JustifyLeft, 255, 255, 255, 255, 1.6, 2.0);
    }

    logic->CommandSelectedByMouseOld = logic->CommandSelectedByMouse;

    commandScreenY+=commandOffsetY;
}
        visuals->Sprites[1251].ScreenX = 320-26;
        visuals->Sprites[1251].ScreenY = 47+130+200;
        visuals->Sprites[1251].ScaleX = 2.1;
        visuals->Sprites[1251].ScaleY = 4.75;
        visuals->DrawSpriteOntoScreenBuffer(1251);

int codeScreenY = 109+200;
int codeOffsetY = 32;
for (int index = logic->CodeDisplayStartIndex; index < logic->CodeDisplayEndIndex; index++)
{
    if (logic->Codes[index].CodeCommandIndex > -1)
    {
        if (logic->CodeSelectedByMouse == index)
        {
            visuals->DrawSentenceOntoScreenBuffer(0, visuals->Commands[ logic->Codes[index].CodeCommandIndex ].CommandTexts, 65+40, codeScreenY, JustifyLeft, 0, 255, 0, 255, 1.6, 2.0);
        }
        else
        {
            visuals->DrawSentenceOntoScreenBuffer(0, visuals->Commands[ logic->Codes[index].CodeCommandIndex ].CommandTexts, 65+40, codeScreenY, JustifyLeft, 255, 255, 255, 255, 1.6, 2.0);
        }
    }

    logic->CodeSelectedByMouseOld = logic->CodeSelectedByMouse;

    codeScreenY+=codeOffsetY;
}

    }

    if (ScreenTransitionStatus == FadeOut && ScreenFadeTransparency == 255)
    {
        ScreenTransitionStatus = FadeAll;
        ScreenToDisplay = SDL_Screen;
    }
}
