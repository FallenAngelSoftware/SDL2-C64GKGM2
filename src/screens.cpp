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

    if (  (ScreenTransitionStatus != FadeNone) || ( logic->DialogToShow == 0 && (logic->DialogToShowOld != logic->DialogToShow) )  )
    {
        ScreenIsDirty = true;
        logic->DialogToShowOld = logic->DialogToShow;
    }

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

    interface->DisplayAllButtonsOnScreenBuffer(0);

    if ( (interface->CurrentInterfaceLevel > 0 && interface->InterfaceLevelBackgroundShown == false) )//|| (logic->DialogToShow == LineNumberSelect) )
    {
        visuals->Sprites[1].ScreenX = 320;
        visuals->Sprites[1].ScreenY = 240;
        visuals->Sprites[1].Transparency = 230;
        visuals->DrawSpriteOntoScreenBuffer(1);
        interface->InterfaceLevelBackgroundShown = true;
    }

    if (logic->DialogToShow == LineNumberSelect)
    {
        ShowLineNumberSelectionDialog();
    }
    else if (logic->DialogToShow == ClearCode)
    {
        ClearCodeDialog();
    }

    interface->DisplayAllButtonsOnScreenBuffer(1);

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

        strcat(visuals->VariableText, " SELECT=");
        sprintf(temp, "%d", logic->CodeSelectorSelected);
        strcat(visuals->VariableText, temp);

        strcat(visuals->VariableText, " EDIT=");
        sprintf(temp, "%d", logic->CodeSelectedForEdit);
        strcat(visuals->VariableText, temp);

        strcat(visuals->VariableText, " LAST=");
        sprintf(temp, "%d", logic->CodeLastLine);
        strcat(visuals->VariableText, temp);

        strcat(visuals->VariableText, " ResizePressY=");
        sprintf(temp, "%d", interface->EditorResizeButtonOriginalPressY);
        strcat(visuals->VariableText, temp);

        visuals->DrawSentenceOntoScreenBuffer(0, visuals->VariableText, 8, 8, JustifyLeft, 255, 255, 255, 255, 1.1, 1.1);
    }

    if (ScreenIsDirty == true || DEBUG == true)
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
void Screens::MoveSelectArrowsOffScreen(void)
{
    interface->Buttons[35].ScreenX = -999;
    interface->Buttons[35].ScreenY = -999;

    interface->Buttons[36].ScreenX = -999;
    interface->Buttons[36].ScreenY = -999;

    interface->Buttons[37].ScreenX = -999;
    interface->Buttons[37].ScreenY = -999;

    interface->Buttons[38].ScreenX = -999;
    interface->Buttons[38].ScreenY = -999;

    interface->Buttons[39].ScreenX = -999;
    interface->Buttons[39].ScreenY = -999;

    interface->Buttons[40].ScreenX = -999;
    interface->Buttons[40].ScreenY = -999;

    interface->Buttons[41].ScreenX = -999;
    interface->Buttons[41].ScreenY = -999;

    interface->Buttons[42].ScreenX = -999;
    interface->Buttons[42].ScreenY = -999;

    interface->Buttons[43].ScreenX = -999;
    interface->Buttons[43].ScreenY = -999;

    interface->Buttons[44].ScreenX = -999;
    interface->Buttons[44].ScreenY = -999;

    interface->Buttons[45].ScreenX = -999;
    interface->Buttons[45].ScreenY = -999;

    interface->Buttons[46].ScreenX = -999;
    interface->Buttons[46].ScreenY = -999;

}

//-------------------------------------------------------------------------------------------------
bool Screens::ThisLineNumberIsAvailable(int lineNumber)
{
    for (int index = 0; index < NumberOfCodes; index++)
    {
        if (lineNumber == logic->Codes[index].CodeCommandLineNumber)  return(false);
    }

    return(true);
}

//-------------------------------------------------------------------------------------------------
void Screens::ShowLineNumberSelectionDialog(void)
{
int lineNumberTotal = 0;

    interface->Buttons[47].ScreenX = 320;
    interface->Buttons[47].ScreenY = 240+130;

    interface->Buttons[48].ScreenX = 320;
    interface->Buttons[48].ScreenY = 240+180;


    interface->Buttons[35].ScreenX = 320-105;
    interface->Buttons[35].ScreenY = (240-20)-75;

    interface->Buttons[36].ScreenX = 320-105;
    interface->Buttons[36].ScreenY = (240-20)+75;

    interface->Buttons[37].ScreenX = 320;
    interface->Buttons[37].ScreenY = (240-20)-75;

    interface->Buttons[38].ScreenX = 320;
    interface->Buttons[38].ScreenY = (240-20)+75;

    interface->Buttons[39].ScreenX = 320+105;
    interface->Buttons[39].ScreenY = (240-20)-75;

    interface->Buttons[40].ScreenX = 320+105;
    interface->Buttons[40].ScreenY = (240-20)+75;

    visuals->DrawSentenceOntoScreenBuffer(0, "ENTER NEW LINE NUMBER:", 320, 65, JustifyCenter, 255, 255, 255, 255, 2.5, 5.0);

    lineNumberTotal = ( (logic->LineNumberArray[0]*100)+(logic->LineNumberArray[1]*10)+logic->LineNumberArray[2] );

    if ( interface->ThisButtonWasPressed == 47 && ThisLineNumberIsAvailable(lineNumberTotal) == true )
    {
        interface->Buttons[47].ScreenX = -999;
        interface->Buttons[47].ScreenY = -999;

        interface->Buttons[48].ScreenX = -999;
        interface->Buttons[48].ScreenY = -999;

        MoveSelectArrowsOffScreen();

        logic->DialogToShow = Nothing;

        interface->CurrentInterfaceLevel = 0;

        interface->InterfaceLevelBackgroundShown = false;

        logic->Codes[logic->CommandDisplayStartIndex+logic->CodeSelectedForLineNumberEdit].CodeCommandLineNumber = ( (logic->LineNumberArray[0]*100)+(logic->LineNumberArray[1]*10)+logic->LineNumberArray[2] );
    }
    else if (interface->ThisButtonWasPressed == 48)
    {
        interface->Buttons[47].ScreenX = -999;
        interface->Buttons[47].ScreenY = -999;

        interface->Buttons[48].ScreenX = -999;
        interface->Buttons[48].ScreenY = -999;

        MoveSelectArrowsOffScreen();

        logic->DialogToShow = Nothing;

        interface->CurrentInterfaceLevel = 0;

        interface->InterfaceLevelBackgroundShown = false;

        logic->Codes[logic->CodeSelectedForLineNumberEdit].CodeCommandLineNumber = -1;
    }

    for (int index = 0; index < 3; index++)
    {
        if ( interface->ThisButtonWasPressed == 35+(2*index) )
        {
            if (logic->LineNumberArray[index] < 9)  logic->LineNumberArray[index]++;
            else  logic->LineNumberArray[index] = 0;
        }
        else if ( interface->ThisButtonWasPressed == 36+(2*index) )
        {
            if (logic->LineNumberArray[index] > 0)  logic->LineNumberArray[index]--;
            else  logic->LineNumberArray[index] = 9;
        }
    }

    char temp[256];
    int screenX = 320 - (105);
    int offsetX = 105;
    Uint8 redColor = 0;
    Uint8 greenColor = 255;
    Uint8 blueColor = 0;
    for (int index = 0; index < 3; index++)
    {
        lineNumberTotal = ( (logic->LineNumberArray[0]*100)+(logic->LineNumberArray[1]*10)+logic->LineNumberArray[2] );

        if ( ThisLineNumberIsAvailable(lineNumberTotal) == false )
        {
            redColor = 255;
            greenColor = 0;
        }

        sprintf(temp, "%i", logic->LineNumberArray[index]);
        strcpy(visuals->VariableText, temp);
        visuals->DrawSentenceOntoScreenBuffer(0, visuals->VariableText, screenX, 240-20, JustifyCenterOnPoint, redColor, greenColor, blueColor, 255, 7.0, 7.0);
        screenX+=offsetX;
    }
}

//-------------------------------------------------------------------------------------------------
void Screens::ClearCodeDialog(void)
{
    interface->Buttons[47].ScreenX = 320;
    interface->Buttons[47].ScreenY = 240+130;

    interface->Buttons[48].ScreenX = 320;
    interface->Buttons[48].ScreenY = 240+180;

    visuals->DrawSentenceOntoScreenBuffer(0, "CLEAR ALL SOURCE CODE?", 320, 65, JustifyCenter, 255, 0, 0, 255, 2.5, 5.0);
    visuals->DrawSentenceOntoScreenBuffer(0, "(WARNING: THIS CAN'T BE UNDONE!)", 320, 65+45, JustifyCenter, 255, 0, 0, 255, 1.5, 1.5);

    if ( interface->ThisButtonWasPressed == 47)
    {
        for (int index = 0; index < NumberOfCodes; index++)
        {
            logic->Codes[index].CodeCommandLineNumber = -1;
            logic->Codes[index].CodeCommandIndex = -1;
            logic->Codes[index].CodeCommandLineActive = false;
        }
        logic->Codes[0].CodeCommandLineActive = true;

        logic->CurrentCodeLine = 0;

        logic->CodeDisplayStartIndex = 0;
        logic->CodeDisplayEndIndex = 5;

        logic->CodeSelectedByMouseOld = -1;
        logic->CodeSelectedByMouse = -1;

        logic->CodeSelectedForEdit = -1;
        logic->CodeSelectorSelected = -1;

        logic->CodeSelectedForLineNumberEdit = -1;

        logic->CalculateCodeLastLine();

        logic->ShowHideCodeSelectLineNumberBoxes();

        interface->Buttons[47].ScreenX = -999;
        interface->Buttons[47].ScreenY = -999;

        interface->Buttons[48].ScreenX = -999;
        interface->Buttons[48].ScreenY = -999;

        MoveSelectArrowsOffScreen();

        logic->DialogToShow = Nothing;

        interface->CurrentInterfaceLevel = 0;

        interface->InterfaceLevelBackgroundShown = false;
    }
    else if (interface->ThisButtonWasPressed == 48)
    {
        interface->Buttons[47].ScreenX = -999;
        interface->Buttons[47].ScreenY = -999;

        interface->Buttons[48].ScreenX = -999;
        interface->Buttons[48].ScreenY = -999;

        MoveSelectArrowsOffScreen();

        logic->DialogToShow = Nothing;

        interface->CurrentInterfaceLevel = 0;

        interface->InterfaceLevelBackgroundShown = false;
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
        interface->CreateButtonWithText(0, true, true, "MENU", 5.0, 2.0, 1200, buttonScreenX, buttonScreenY, 255, 255, 255, 255, 0.3, 1.0);
        buttonScreenY+=buttonOffsetY;
        interface->CreateButtonWithText(0, true, true, "FILE", 5.0, 2.0, 1200, buttonScreenX, buttonScreenY, 255, 255, 255, 255, 0.3, 1.0);
        buttonScreenY+=buttonOffsetY;
        interface->CreateButtonWithText(0, true, true, "CLR", 5.0, 2.0, 1200, buttonScreenX, buttonScreenY, 255, 255, 255, 255, 0.3, 1.0);
        buttonScreenY+=buttonOffsetY;
        interface->CreateButtonWithText(0, true, true, "RUN", 5.0, 2.0, 1200, buttonScreenX, buttonScreenY, 255, 255, 255, 255, 0.3, 1.0);
        buttonScreenY+=buttonOffsetY;
        interface->CreateButtonWithText(0, true, true, "INS", 5.0, 2.0, 1200, buttonScreenX, buttonScreenY, 255, 255, 255, 255, 0.3, 1.0);
        interface->Buttons[4].BlockPressing = true;
        buttonScreenY+=buttonOffsetY;
        interface->CreateButtonWithText(0, true, true, "DEL", 5.0, 2.0, 1200, buttonScreenX, buttonScreenY, 255, 255, 255, 255, 0.3, 1.0);
        interface->Buttons[5].BlockPressing = true;
        buttonScreenY+=buttonOffsetY;
        interface->CreateButtonWithText(0, true, true, "FIND", 5.0, 2.0, 1200, buttonScreenX, buttonScreenY, 255, 255, 255, 255, 0.3, 1.0);
        buttonScreenY+=buttonOffsetY;
        interface->CreateButtonWithText(0, true, true, "COPY", 5.0, 2.0, 1200, buttonScreenX, buttonScreenY, 255, 255, 255, 255, 0.3, 1.0);

        interface->CommandUpArrayScreenY = 132;
        interface->CommandUpArrowScaleY = 1.2;
        interface->CommandDownArrowScreenY = 132+90;
        interface->CommandDownArrowScaleY = 1.2;

        interface->CodeUpArrowScreenY = 132+199;
        interface->CodeUpArrowScaleY = 1.2;
        interface->CodeDownArrowScreenY = 132+90+199;
        interface->CodeDownArrowScaleY = 1.2;

        interface->CreateButtonWithText(0, false, true, "", 1.0, 1.0, 1210, 16, interface->CommandUpArrayScreenY, 255, 255, 255, 255, 0.35, interface->CommandUpArrowScaleY);
        interface->CreateButtonWithText(0, false, true, "", 1.0, 1.0, 1215, 16, interface->CommandDownArrowScreenY, 255, 255, 255, 255, 0.35, interface->CommandDownArrowScaleY);

        interface->CreateButtonWithText(0, false, true, "", 1.0, 1.0, 1211, 16, interface->CodeUpArrowScreenY, 255, 255, 255, 255, 0.35, interface->CodeUpArrowScaleY);
        interface->CreateButtonWithText(0, false, true, "", 1.0, 1.0, 1216, 16, interface->CodeDownArrowScreenY, 255, 255, 255, 255, 0.35, interface->CodeDownArrowScaleY);

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

        logic->CodeSelectedForLineNumberEdit = -1;

        for (int index = 0; index < 3; index++)
        {
            logic->LineNumberArray[index] = 0;
        }

        int codeEditScreenY = 109+200-2;
        int codeEditOffsetY = 32-10;
        interface->CreateButtonWithText(0, true, true, "", 1.0, 1.0, 1220, 43, codeEditScreenY, 255, 255, 255, 255, 0.5, 1.0);
        codeEditScreenY+=codeEditOffsetY;
        interface->CreateButtonWithText(0, true, true, "", 1.0, 1.0, 1220, 43, codeEditScreenY, 255, 255, 255, 255, 0.5, 1.0);
        codeEditScreenY+=codeEditOffsetY;
        interface->CreateButtonWithText(0, true, true, "", 1.0, 1.0, 1220, 43, codeEditScreenY, 255, 255, 255, 255, 0.5, 1.0);
        codeEditScreenY+=codeEditOffsetY;
        interface->CreateButtonWithText(0, true, true, "", 1.0, 1.0, 1220, 43, codeEditScreenY, 255, 255, 255, 255, 0.5, 1.0);
        codeEditScreenY+=codeEditOffsetY;
        interface->CreateButtonWithText(0, true, true, "", 1.0, 1.0, 1220, 43, codeEditScreenY, 255, 255, 255, 255, 0.5, 1.0);

        codeEditScreenY+=codeEditOffsetY;
        interface->CreateButtonWithText(0, true, true, "", 1.0, 1.0, 1220, 43, codeEditScreenY, 255, 255, 255, 255, 0.5, 1.0);
        codeEditScreenY+=codeEditOffsetY;
        interface->CreateButtonWithText(0, true, true, "", 1.0, 1.0, 1220, 43, codeEditScreenY, 255, 255, 255, 255, 0.5, 1.0);
        codeEditScreenY+=codeEditOffsetY;
        interface->CreateButtonWithText(0, true, true, "", 1.0, 1.0, 1220, 43, codeEditScreenY, 255, 255, 255, 255, 0.5, 1.0);
        codeEditScreenY+=codeEditOffsetY;
        interface->CreateButtonWithText(0, true, true, "", 1.0, 1.0, 1220, 43, codeEditScreenY, 255, 255, 255, 255, 0.5, 1.0);
        codeEditScreenY+=codeEditOffsetY;
        interface->CreateButtonWithText(0, true, true, "", 1.0, 1.0, 1220, 43, codeEditScreenY, 255, 255, 255, 255, 0.5, 1.0);
        codeEditScreenY+=codeEditOffsetY;
        interface->CreateButtonWithText(0, true, true, "", 1.0, 1.0, 1220, 43, codeEditScreenY, 255, 255, 255, 255, 0.5, 1.0);


        codeEditScreenY = 109+200-2;
        codeEditOffsetY = 32-10;
        interface->CreateButtonWithText(0, true, true, "", 1.0, 1.0, 1221, 73, codeEditScreenY, 255, 255, 255, 127, 1.0, 1.0);
        codeEditScreenY+=codeEditOffsetY;
        interface->CreateButtonWithText(0, true, true, "", 1.0, 1.0, 1221, 73, codeEditScreenY, 255, 255, 255, 127, 1.0, 1.0);
        codeEditScreenY+=codeEditOffsetY;
        interface->CreateButtonWithText(0, true, true, "", 1.0, 1.0, 1221, 73, codeEditScreenY, 255, 255, 255, 127, 1.0, 1.0);
        codeEditScreenY+=codeEditOffsetY;
        interface->CreateButtonWithText(0, true, true, "", 1.0, 1.0, 1221, 73, codeEditScreenY, 255, 255, 255, 127, 1.0, 1.0);
        codeEditScreenY+=codeEditOffsetY;
        interface->CreateButtonWithText(0, true, true, "", 1.0, 1.0, 1221, 73, codeEditScreenY, 255, 255, 255, 127, 1.0, 1.0);

        codeEditScreenY+=codeEditOffsetY;
        interface->CreateButtonWithText(0, true, true, "", 1.0, 1.0, 1221, 73, codeEditScreenY, 255, 255, 255, 127, 1.0, 1.0);
        codeEditScreenY+=codeEditOffsetY;
        interface->CreateButtonWithText(0, true, true, "", 1.0, 1.0, 1221, 73, codeEditScreenY, 255, 255, 255, 127, 1.0, 1.0);
        codeEditScreenY+=codeEditOffsetY;
        interface->CreateButtonWithText(0, true, true, "", 1.0, 1.0, 1221, 73, codeEditScreenY, 255, 255, 255, 127, 1.0, 1.0);
        codeEditScreenY+=codeEditOffsetY;
        interface->CreateButtonWithText(0, true, true, "", 1.0, 1.0, 1221, 73, codeEditScreenY, 255, 255, 255, 127, 1.0, 1.0);
        codeEditScreenY+=codeEditOffsetY;
        interface->CreateButtonWithText(0, true, true, "", 1.0, 1.0, 1221, 73, codeEditScreenY, 255, 255, 255, 127, 1.0, 1.0);
        codeEditScreenY+=codeEditOffsetY;
        interface->CreateButtonWithText(0, true, true, "", 1.0, 1.0, 1221, 73, codeEditScreenY, 255, 255, 255, 127, 1.0, 1.0);


        interface->CreateButtonWithText(0, false, false, "", 1.0, 1.0, 1150, 320-26, 240+36, 255, 255, 255, 255, 1.0, 1.0);

        interface->CreateButtonWithText(1, true, false, "", 1.0, 1.0, 1210, -999, -999, 255, 255, 255, 255, 1.0, 1.0);
        interface->CreateButtonWithText(1, true, false, "", 1.0, 1.0, 1215, -999, -999, 255, 255, 255, 255, 1.0, 1.0);
        interface->CreateButtonWithText(1, true, false, "", 1.0, 1.0, 1210, -999, -999, 255, 255, 255, 255, 1.0, 1.0);
        interface->CreateButtonWithText(1, true, false, "", 1.0, 1.0, 1215, -999, -999, 255, 255, 255, 255, 1.0, 1.0);
        interface->CreateButtonWithText(1, true, false, "", 1.0, 1.0, 1210, -999, -999, 255, 255, 255, 255, 1.0, 1.0);
        interface->CreateButtonWithText(1, true, false, "", 1.0, 1.0, 1215, -999, -999, 255, 255, 255, 255, 1.0, 1.0);
        interface->CreateButtonWithText(1, true, false, "", 1.0, 1.0, 1210, -999, -999, 255, 255, 255, 255, 1.0, 1.0);
        interface->CreateButtonWithText(1, true, false, "", 1.0, 1.0, 1215, -999, -999, 255, 255, 255, 255, 1.0, 1.0);
        interface->CreateButtonWithText(1, true, false, "", 1.0, 1.0, 1210, -999, -999, 255, 255, 255, 255, 1.0, 1.0);
        interface->CreateButtonWithText(1, true, false, "", 1.0, 1.0, 1215, -999, -999, 255, 255, 255, 255, 1.0, 1.0);
        interface->CreateButtonWithText(1, true, false, "", 1.0, 1.0, 1210, -999, -999, 255, 255, 255, 255, 1.0, 1.0);
        interface->CreateButtonWithText(1, true, false, "", 1.0, 1.0, 1215, -999, -999, 255, 255, 255, 255, 1.0, 1.0);

        interface->CreateButtonWithText(1, true, true, "OK", 4.0, 2.0, 1200, -999, -999, 100, 255, 100, 255, 1.0, 1.0);
        interface->CreateButtonWithText(1, true, true, "CANCEL", 4.0, 2.0, 1200, -999, -999, 255, 100, 100, 255, 1.0, 1.0);

        logic->DialogToShow = Nothing;

        logic->CommandBoxMaxY = 7;
        logic->CodeBoxMaxY = 7;

        logic->CodeBoxOffsetY = 0;

        interface->CommandBoxScaleY = 5;
        interface->CodeBoxScaleY = 5;

        interface->CodeBoxScreenY = 473;

        logic->ShowHideCodeSelectLineNumberBoxes();

        ScreenTransitionStatus = FadeIn;
    }

    logic->RunCodeEditor();

    if (ScreenIsDirty == true)
    {
        visuals->ClearScreenBufferWithColor(100, 100, 100, 255);

        visuals->Sprites[100].ScreenX = 320;
        visuals->Sprites[100].ScreenY = 240;
        visuals->DrawSpriteOntoScreenBuffer(100);

        visuals->Sprites[1249].ScreenX = 320-26;
        visuals->Sprites[1249].ScreenY = 47;
        visuals->Sprites[1249].ScaleX = 2.1;
        visuals->Sprites[1249].ScaleY = 1.4;
        visuals->DrawSpriteOntoScreenBuffer(1249);

        visuals->Sprites[1250].ScreenX = 320-26;
        visuals->Sprites[1250].ScreenY = 86;
        visuals->Sprites[1250].ScaleX = 2.1;
        visuals->Sprites[1250].ScaleY = interface->CommandBoxScaleY;
        visuals->DrawSpriteOntoScreenBuffer(1250);

visuals->DrawSentenceOntoScreenBuffer(1, "NAME-", 435, 35, JustifyLeft, 255, 255, 255, 255, 1.0, 2.0);
visuals->DrawSentenceOntoScreenBuffer(1, "INTRO", 640-95, 35, JustifyRight, 255, 255, 255, 255, 1.0, 2.0);

visuals->DrawSentenceOntoScreenBuffer(1, "FREE-", 435, 35+23, JustifyLeft, 255, 255, 255, 255, 1.0, 2.0);
visuals->DrawSentenceOntoScreenBuffer(1, "1999", 640-95, 35+23, JustifyRight, 255, 255, 255, 255, 1.0, 2.0);

int commandScreenY = 107;
int commandOffsetY = 32-10;
for (int index = logic->CommandDisplayStartIndex; index < logic->CommandDisplayEndIndex; index++)
{
    if (logic->CommandSelectedByMouse == index)
    {
        if (interface->CurrentInterfaceLevel == 0)  visuals->DrawSentenceOntoScreenBuffer(1, visuals->Commands[index].CommandTexts, 65, commandScreenY, JustifyLeft, 0, 255, 0, 255, 1.6, 2.0);
        else  visuals->DrawSentenceOntoScreenBuffer(1, visuals->Commands[index].CommandTexts, 65, commandScreenY, JustifyLeft, 255, 255, 255, 255, 1.6, 2.0);
    }
    else
    {
        visuals->DrawSentenceOntoScreenBuffer(1, visuals->Commands[index].CommandTexts, 65, commandScreenY, JustifyLeft, 255, 255, 255, 255, 1.6, 2.0);
    }

    logic->CommandSelectedByMouseOld = logic->CommandSelectedByMouse;

    commandScreenY+=commandOffsetY;
}
        visuals->Sprites[1251].ScreenX = 320-26;
        visuals->Sprites[1251].ScreenY = interface->CodeBoxScreenY;//86+200;
        visuals->Sprites[1251].ScaleX = 2.1;
        visuals->Sprites[1251].ScaleY = interface->CodeBoxScaleY;
        visuals->DrawSpriteOntoScreenBuffer(1251);

int codeScreenY = 109+200-2+logic->CodeBoxOffsetY;
int codeOffsetY = 32-10;
for (int index = logic->CodeDisplayStartIndex; index < logic->CodeDisplayEndIndex; index++)
{
    if ( (logic->Codes[index].CodeCommandIndex > -1) || (logic->Codes[index].CodeCommandLineNumber > -1) )
    {
        if (logic->Codes[index].CodeCommandLineNumber > -1)
        {
            char temp[256];
            sprintf(temp, "%i", logic->Codes[index].CodeCommandLineNumber);
            strcpy(visuals->VariableText, temp);
            visuals->DrawSentenceOntoScreenBuffer(0, visuals->VariableText, 90, codeScreenY, JustifyRight, 0, 255, 255, 255, 1.2, 1.5);
        }

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
