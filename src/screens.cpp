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
    if (logic->CodeSelectedByMouseOld != logic->CodeSelectedByMouse)  ScreenIsDirty = true;

    SDL_GetWindowSize(visuals->Window, &windowWidth, &windowHeight);
    if (windowWidth != visuals->WindowWidthCurrent || windowHeight != visuals->WindowHeightCurrent)
    {
        visuals->WindowWidthCurrent = windowWidth;
        visuals->WindowHeightCurrent = windowHeight;

        ScreenIsDirty = true;
    }

    if (logic->DialogToShow == DialogFindLineNumber && LineNumberFoundNew != LineNumberFoundOld)
    {
        ScreenIsDirty = true;
        LineNumberFoundOld = LineNumberFoundNew;
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

    if ( (interface->CurrentInterfaceLevel == 1 && interface->InterfaceLevelBackgroundShown == false) )
    {
        visuals->Sprites[1].ScreenX = 320;
        visuals->Sprites[1].ScreenY = 240;
        visuals->Sprites[1].Transparency = 230;
        visuals->DrawSpriteOntoScreenBuffer(1);
        interface->InterfaceLevelBackgroundShown = true;
    }

    if (logic->DialogToShow == DialogLineNumberSelect)
    {
        ShowLineNumberSelectionDialog();
    }
    else if (logic->DialogToShow == DialogClearCode)
    {
        ShowClearCodeDialog();
    }
    else if (logic->DialogToShow == DialogFindLineNumber)
    {
        ShowFindLineNumberDialog();
    }
    else if (logic->DialogToShow == DialogEdit)
    {
        ShowEditDialog();
    }

    interface->DisplayAllButtonsOnScreenBuffer(1);
    interface->DisplayAllButtonsOnScreenBuffer(2);

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

        strcat(visuals->VariableText, " EDITLOC=");
        sprintf(temp, "%d", logic->EditLocation);
        strcat(visuals->VariableText, temp);

        visuals->DrawSentenceOntoScreenBuffer(0, visuals->VariableText, 8, 8, JustifyLeft, 255, 255, 255, 255, 1, 1);
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
void Screens::MoveSelectArrowsAndButtonsOffScreen(void)
{
    for ( int index = (interface->CodeLineSelectorButtonsEnd+2); index < (interface->CodeLineSelectorButtonsEnd+1+12); index++ )
    {
        interface->Buttons[index].ScreenX = -999;
        interface->Buttons[index].ScreenY = -999;
    }

    for ( int index = (interface->CodeLineSelectorButtonsEnd+14); index < (interface->CodeLineSelectorButtonsEnd+14+4); index++ )
    {
        interface->Buttons[index].ScreenX = -999;
        interface->Buttons[index].ScreenY = -999;
    }
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

    interface->Buttons[interface->CodeLineSelectorButtonsEnd+14].ScreenX = 320;
    interface->Buttons[interface->CodeLineSelectorButtonsEnd+14].ScreenY = 240+130;

    if (logic->Codes[logic->CommandDisplayStartIndex+logic->CodeSelectedForLineNumberEdit].CodeCommandLineNumber > -1)
    {
        interface->Buttons[interface->CodeLineSelectorButtonsEnd+15].ScreenX = 320;
        interface->Buttons[interface->CodeLineSelectorButtonsEnd+15].ScreenY = 240+170;
    }

    interface->Buttons[interface->CodeLineSelectorButtonsEnd+16].ScreenX = 320;
    interface->Buttons[interface->CodeLineSelectorButtonsEnd+16].ScreenY = 240+210;

    int buttonScreenX = 320-105;
    int buttonScreenXoffset = 105;
    for ( int index = (interface->CodeLineSelectorButtonsEnd+2); index < (interface->CodeLineSelectorButtonsEnd+8); index+=2 )
    {
        interface->Buttons[index].ScreenX = buttonScreenX;
        interface->Buttons[index].ScreenY = (240-20)-75;

        interface->Buttons[index+1].ScreenX = buttonScreenX;
        interface->Buttons[index+1].ScreenY = (240-20)+75;

        buttonScreenX+=buttonScreenXoffset;
    }

    visuals->DrawSentenceOntoScreenBuffer(0, "ENTER NEW LINE NUMBER:", 320, 65, JustifyCenter, 255, 255, 255, 255, 2.5, 5.0);

    lineNumberTotal = ( (logic->LineNumberArray[0]*100)+(logic->LineNumberArray[1]*10)+logic->LineNumberArray[2] );

    if ( interface->ThisButtonWasPressed == (interface->CodeLineSelectorButtonsEnd+14) && ThisLineNumberIsAvailable(lineNumberTotal) == true )
    {
        MoveSelectArrowsAndButtonsOffScreen();

        logic->DialogToShow = DialogNothing;

        interface->CurrentInterfaceLevel = 0;

        interface->InterfaceLevelBackgroundShown = false;

        logic->Codes[logic->CommandDisplayStartIndex+logic->CodeSelectedForLineNumberEdit].CodeCommandLineNumber = ( (logic->LineNumberArray[0]*100)+(logic->LineNumberArray[1]*10)+logic->LineNumberArray[2] );
    }
    else if ( interface->ThisButtonWasPressed == (interface->CodeLineSelectorButtonsEnd+15) )
    {
        MoveSelectArrowsAndButtonsOffScreen();

        logic->DialogToShow = DialogNothing;

        interface->CurrentInterfaceLevel = 0;

        interface->InterfaceLevelBackgroundShown = false;

        logic->Codes[logic->CodeSelectedForLineNumberEdit].CodeCommandLineNumber = -1;
    }
    else if ( interface->ThisButtonWasPressed == (interface->CodeLineSelectorButtonsEnd+16) )
    {
        MoveSelectArrowsAndButtonsOffScreen();

        logic->DialogToShow = DialogNothing;

        interface->CurrentInterfaceLevel = 0;

        interface->InterfaceLevelBackgroundShown = false;
    }

    for (int index = 0; index < 3; index++)
    {
        if ( interface->ThisButtonWasPressed == (interface->CodeLineSelectorButtonsEnd+2)+(2*index) )
        {
            if (logic->LineNumberArray[index] < 9)  logic->LineNumberArray[index]++;
            else  logic->LineNumberArray[index] = 0;
        }
        else if ( interface->ThisButtonWasPressed == (interface->CodeLineSelectorButtonsEnd+3)+(2*index) )
        {
            if (logic->LineNumberArray[index] > 0)  logic->LineNumberArray[index]--;
            else  logic->LineNumberArray[index] = 9;
        }
    }

    lineNumberTotal = ( (logic->LineNumberArray[0]*100)+(logic->LineNumberArray[1]*10)+logic->LineNumberArray[2] );

    char temp[256];
    int screenX = 320 - (105);
    int offsetX = 105;
    Uint8 redColor = 0;
    Uint8 greenColor = 255;
    Uint8 blueColor = 0;
    for (int index = 0; index < 3; index++)
    {
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
void Screens::ShowClearCodeDialog(void)
{
    interface->Buttons[interface->CodeLineSelectorButtonsEnd+14].ScreenX = 320;
    interface->Buttons[interface->CodeLineSelectorButtonsEnd+14].ScreenY = 240+130;

    interface->Buttons[interface->CodeLineSelectorButtonsEnd+16].ScreenX = 320;
    interface->Buttons[interface->CodeLineSelectorButtonsEnd+16].ScreenY = 240+210;

    visuals->DrawSentenceOntoScreenBuffer(0, "CLEAR ALL SOURCE CODE?", 320, 65, JustifyCenter, 255, 0, 0, 255, 2.5, 5.0);
    int textY = 65+60;
    int textYoffset = 45;
    visuals->DrawSentenceOntoScreenBuffer(0, " WARNING:", 320, textY, JustifyCenter, 255, 0, 0, 255, 2.5, 2.5);
    textY+=textYoffset;
    visuals->DrawSentenceOntoScreenBuffer(0, "THIS", 320, textY, JustifyCenter, 255, 0, 0, 255, 2.5, 2.5);
    textY+=textYoffset;
    visuals->DrawSentenceOntoScreenBuffer(0, "CAN'T", 320, textY, JustifyCenter, 255, 0, 0, 255, 2.5, 2.5);
    textY+=textYoffset;
    visuals->DrawSentenceOntoScreenBuffer(0, "BE", 320, textY, JustifyCenter, 255, 0, 0, 255, 2.5, 2.5);
    textY+=textYoffset;
    visuals->DrawSentenceOntoScreenBuffer(0, "UNDONE", 320, textY, JustifyCenter, 255, 0, 0, 255, 2.5, 2.5);

    if ( interface->ThisButtonWasPressed == (interface->CodeLineSelectorButtonsEnd+14) )
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

        interface->SetupCodingWindows();

        logic->ShowHideCodeSelectLineNumberBoxes();

        MoveSelectArrowsAndButtonsOffScreen();

        logic->DialogToShow = DialogNothing;

        interface->CurrentInterfaceLevel = 0;

        interface->InterfaceLevelBackgroundShown = false;
    }
    else if ( interface->ThisButtonWasPressed == (interface->CodeLineSelectorButtonsEnd+16) )
    {
        MoveSelectArrowsAndButtonsOffScreen();

        logic->DialogToShow = DialogNothing;

        interface->CurrentInterfaceLevel = 0;

        interface->InterfaceLevelBackgroundShown = false;
    }
}

//-------------------------------------------------------------------------------------------------
void Screens::ShowFindLineNumberDialog(void)
{
int lineNumberTotal = 0;

    interface->Buttons[interface->CodeLineSelectorButtonsEnd+17].ScreenX = 320;
    interface->Buttons[interface->CodeLineSelectorButtonsEnd+17].ScreenY = 50;

    visuals->DrawSentenceOntoScreenBuffer(0, "FIND", 45, 52-15, JustifyLeft, 0, 255, 0, 255, 2.0, 2.0);
    visuals->DrawSentenceOntoScreenBuffer(0, "LINE", 45, 52+15, JustifyLeft, 0, 255, 0, 255, 2.0, 2.0);
    visuals->DrawSentenceOntoScreenBuffer(0, "#:", 45+85, 52, JustifyLeft, 0, 255, 0, 255, 4.0, 4.0);

    for ( int index = (interface->CodeLineSelectorButtonsEnd+2); index < (interface->CodeLineSelectorButtonsEnd+8); index++ )
    {
        interface->Buttons[index].ScaleX = 0.3;
        interface->Buttons[index].ScaleY = 0.3;
        interface->Buttons[index].InterfaceLevel = 2;
    }

    int buttonScreenX = 200;
    int buttonScreenXoffset = 30;
    for ( int index = (interface->CodeLineSelectorButtonsEnd+2); index < (interface->CodeLineSelectorButtonsEnd+8); index+=2 )
    {
        interface->Buttons[index].ScreenX = buttonScreenX;
        interface->Buttons[index].ScreenY = 51-20;

        interface->Buttons[index+1].ScreenX = buttonScreenX;
        interface->Buttons[index+1].ScreenY = 51+20;

        buttonScreenX+=buttonScreenXoffset;
    }

    if ( interface->ThisButtonWasPressed == (interface->CodeLineSelectorButtonsEnd+17) )
    {
        interface->CurrentInterfaceLevel = 0;

        logic->DialogToShow = DialogNothing;

        for ( int index = (interface->CodeLineSelectorButtonsEnd+2); index < (interface->CodeLineSelectorButtonsEnd+8); index++ )
        {
            interface->Buttons[index].ScaleX = 1.0;
            interface->Buttons[index].ScaleY = 1.0;
            interface->Buttons[index].InterfaceLevel = 1;
        }

        MoveSelectArrowsAndButtonsOffScreen();
    }

    for (int index = 0; index < 3; index++)
    {
        if ( interface->ThisButtonWasPressed == (interface->CodeLineSelectorButtonsEnd+2)+(2*index) )
        {
            if (logic->LineNumberArray[index] < 9)  logic->LineNumberArray[index]++;
            else  logic->LineNumberArray[index] = 0;
        }
        else if ( interface->ThisButtonWasPressed == (interface->CodeLineSelectorButtonsEnd+3)+(2*index) )
        {
            if (logic->LineNumberArray[index] > 0)  logic->LineNumberArray[index]--;
            else  logic->LineNumberArray[index] = 9;
        }
    }

    lineNumberTotal = ( (logic->LineNumberArray[0]*100)+(logic->LineNumberArray[1]*10)+logic->LineNumberArray[2] );

    char temp[256];
    int screenX = 200;
    int offsetX = 30;
    Uint8 redColor = 0;
    Uint8 greenColor = 255;
    Uint8 blueColor = 0;
    for (int index = 0; index < 3; index++)
    {
        if ( ThisLineNumberIsAvailable(lineNumberTotal) == true )
        {
            redColor = 255;
            greenColor = 0;
        }

        sprintf(temp, "%i", logic->LineNumberArray[index]);
        strcpy(visuals->VariableText, temp);
        visuals->DrawSentenceOntoScreenBuffer(0, visuals->VariableText, screenX, 50, JustifyCenterOnPoint, redColor, greenColor, blueColor, 255, 2.0, 2.0);
        screenX+=offsetX;
    }

    for (int index = 0; index < NumberOfCodes; index++)
    {
        if (logic->Codes[index].CodeCommandLineNumber == lineNumberTotal)
        {
            logic->CodeDisplayStartIndex = index;
            logic->CodeDisplayEndIndex = logic->CodeDisplayStartIndex+logic->CodeBoxMaxY;

            LineNumberFoundNew = index;

            logic->ClearAllCodeSelectors();

            logic->CodeSelectorSelected = 0;
            logic->CodeSelectedForEdit = logic->CodeDisplayStartIndex;

            logic->ShowHideCodeSelectLineNumberBoxes();
        }
    }
}

//-------------------------------------------------------------------------------------------------
void Screens::ShowEditDialog(void)
{
    if (interface->EditStatus == EditStatusFirst)
    {
        visuals->DrawSentenceOntoScreenBuffer(1, "SELECT FIRST LINE:", 45, 52-15+15, JustifyLeft, 100, 255, 100, 255, 2.0, 2.0);
    }
    else if (interface->EditStatus == EditStatusLast)
    {
        visuals->DrawSentenceOntoScreenBuffer(1, "SELECT LAST LINE:", 45, 52-15+15, JustifyLeft, 100, 255, 100, 255, 2.0, 2.0);
    }
    else if (interface->EditStatus == EditStatusCommand)
    {
        visuals->DrawSentenceOntoScreenBuffer(1, "SELECT COMMAND:", 45, 52-15+15, JustifyLeft, 100, 255, 100, 255, 2.0, 2.0);
    }
    else if (interface->EditStatus >= EditStatusLocationMove)
    {
        visuals->DrawSentenceOntoScreenBuffer(1, "SELECT LOCATION:", 45, 52-15+15, JustifyLeft, 100, 255, 100, 255, 2.0, 2.0);
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
        interface->CreateButtonWithText(0, true, true, "EDIT", 5.0, 2.0, 1200, buttonScreenX, buttonScreenY, 255, 255, 255, 255, 0.3, 1.0);

        interface->CreateButtonWithText(0, false, true, "", 1.0, 1.0, 1210, 16, 132, 255, 255, 255, 255, 0.35, 0.35);
        interface->CreateButtonWithText(0, false, true, "", 1.0, 1.0, 1215, 16, 132+90, 255, 255, 255, 255, 0.35, 0.35);

        interface->CreateButtonWithText(0, false, true, "", 1.0, 1.0, 1211, 16, 132+199, 255, 255, 255, 255, 0.35, 0.35);
        interface->CreateButtonWithText(0, false, true, "", 1.0, 1.0, 1216, 16, 132+90+199, 255, 255, 255, 255, 0.35, 0.35);

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

        int codeEditScreenY = 109+200-2+5-10;
        int codeEditOffsetY = 32-10-4;
        interface->CodeSelectorButtonsStart = interface->CreateButtonWithText(0, true, true, "", 1.0, 1.0, 1220, 43, codeEditScreenY, 255, 255, 255, 255, 0.5, 1.0);
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
        interface->CodeSelectorButtonsEnd = interface->CreateButtonWithText(0, true, true, "", 1.0, 1.0, 1220, 43, codeEditScreenY, 255, 255, 255, 255, 0.5, 1.0);

        codeEditScreenY = 109+200-2+5-10;
        codeEditOffsetY = 32-10-4;
        interface->CodeLineSelectorButtonsStart = interface->CreateButtonWithText(0, true, true, "", 1.0, 1.0, 1221, 73, codeEditScreenY, 255, 255, 255, 127, 1.0, 1.0);
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
        interface->CodeLineSelectorButtonsEnd = interface->CreateButtonWithText(0, true, true, "", 1.0, 1.0, 1221, 73, codeEditScreenY, 255, 255, 255, 127, 1.0, 1.0);

        interface->CreateButtonWithText(0, false, false, "", 1.0, 1.0, 1150, 320-26, 240+36+5, 255, 255, 255, 255, 1.0, 1.0);

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
        interface->CreateButtonWithText(1, true, true, "DELETE", 4.0, 2.0, 1200, -999, -999, 255, 255, 100, 255, 1.0, 1.0);
        interface->CreateButtonWithText(1, true, true, "CANCEL", 4.0, 2.0, 1200, -999, -999, 255, 100, 100, 255, 1.0, 1.0);

        interface->CreateButtonWithText(2, true, true, "OK", 8.5, 3.0, 1200, -999, -999, 100, 255, 100, 255, 0.25, 1.25);

        logic->DialogToShow = DialogNothing;

        logic->CommandBoxMaxY = 7;
        logic->CodeBoxMaxY = 7;

        logic->CodeBoxOffsetY = 0;

        logic->ShowHideCodeSelectLineNumberBoxes();

        interface->SetupCodingWindows();

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
        visuals->Sprites[1249].ScreenY = 47+5;
        visuals->Sprites[1249].ScaleX = 2.1;
        visuals->Sprites[1249].ScaleY = 1.8;
        visuals->DrawSpriteOntoScreenBuffer(1249);

visuals->DrawSentenceOntoScreenBuffer(1, "NAME-", 435, 40, JustifyLeft, 255, 255, 255, 255, 1.0, 2.0);
visuals->DrawSentenceOntoScreenBuffer(1, "INTRO", 640-95, 40, JustifyRight, 255, 255, 255, 255, 1.0, 2.0);

visuals->DrawSentenceOntoScreenBuffer(1, "FREE-", 435, 40+23, JustifyLeft, 255, 255, 255, 255, 1.0, 2.0);
visuals->DrawSentenceOntoScreenBuffer(1, "1999", 640-95, 40+23, JustifyRight, 255, 255, 255, 255, 1.0, 2.0);

        int commandScreenY = 107-10;
        int commandOffsetY = 32-10-4;
        for (int index = logic->CommandDisplayStartIndex; index < logic->CommandDisplayEndIndex; index++)
        {
            if (index == logic->CommandDisplayStartIndex)
            {
                interface->Buttons[8].ScreenY = commandScreenY;
            }

            visuals->Sprites[1300].ScreenX = 320-26;
            visuals->Sprites[1300].ScreenY = commandScreenY;
            visuals->Sprites[1300].RedHue = 255;
            visuals->Sprites[1300].GreenHue = 255;
            visuals->Sprites[1300].BlueHue = 255;
            visuals->DrawSpriteOntoScreenBuffer(1300);

            if (logic->CommandSelectedByMouse == index)
            {
                if (interface->CurrentInterfaceLevel == 0)  visuals->DrawSentenceOntoScreenBuffer(1, visuals->Commands[index].CommandTexts, 65, commandScreenY, JustifyLeft, 0, 255, 0, 255, 1.6, 1.6);
                else  visuals->DrawSentenceOntoScreenBuffer(1, visuals->Commands[index].CommandTexts, 65, commandScreenY, JustifyLeft, 255, 255, 255, 255, 1.6, 1.6);
            }
            else
            {
                visuals->DrawSentenceOntoScreenBuffer(1, visuals->Commands[index].CommandTexts, 65, commandScreenY, JustifyLeft, 255, 255, 255, 255, 1.6, 1.6);
            }

            logic->CommandSelectedByMouseOld = logic->CommandSelectedByMouse;

            commandScreenY+=commandOffsetY;
        }
        interface->Buttons[9].ScreenY = (commandScreenY-17);


        int codeScreenY = 109+200-2+logic->CodeBoxOffsetY + 5 - 10;
        int codeOffsetY = 32-10-4;
        for (int index = logic->CodeDisplayStartIndex; index < logic->CodeDisplayEndIndex; index++)
        {
            if (index == logic->CodeDisplayStartIndex)
            {
                interface->Buttons[10].ScreenY = codeScreenY;
            }

            visuals->Sprites[1310].ScreenX = 320-26;
            visuals->Sprites[1310].ScreenY = codeScreenY;
            visuals->DrawSpriteOntoScreenBuffer(1310);

            if (interface->EditStatus == EditStatusLast)
            {
                if (index == logic->EditLineSelectedByMouse)
                {
                    visuals->Sprites[1300].ScreenX = 320-26;
                    visuals->Sprites[1300].ScreenY = codeScreenY;
                    visuals->Sprites[1300].RedHue = 100;
                    visuals->Sprites[1300].GreenHue = 200;
                    visuals->Sprites[1300].BlueHue = 100;
                    visuals->DrawSpriteOntoScreenBuffer(1300);
                }

                if ( (logic->EditLineSelectedByMouse > -1) && (logic->EditFirstLine < logic->EditLineSelectedByMouse) )
                {
                    if (index >= logic->EditFirstLine && index <= logic->EditLineSelectedByMouse)
                    {
                        visuals->Sprites[1300].ScreenX = 320-26;
                        visuals->Sprites[1300].ScreenY = codeScreenY;
                        visuals->Sprites[1300].RedHue = 100;
                        visuals->Sprites[1300].GreenHue = 200;
                        visuals->Sprites[1300].BlueHue = 100;
                        visuals->DrawSpriteOntoScreenBuffer(1300);
                    }
                }
                if ( (logic->EditLineSelectedByMouse > -1) && (logic->EditFirstLine > logic->EditLineSelectedByMouse) )
                {
                    if (index <= logic->EditFirstLine && index >= logic->EditLineSelectedByMouse)
                    {
                        visuals->Sprites[1300].ScreenX = 320-26;
                        visuals->Sprites[1300].ScreenY = codeScreenY;
                        visuals->Sprites[1300].RedHue = 100;
                        visuals->Sprites[1300].GreenHue = 200;
                        visuals->Sprites[1300].BlueHue = 100;
                        visuals->DrawSpriteOntoScreenBuffer(1300);
                    }
                }
            }
            else if (interface->EditStatus == EditStatusCommand || interface->EditStatus >= EditStatusLocationMove)
            {
                if ( (logic->EditFirstLine <= logic->EditLastLine) && (index >= logic->EditFirstLine && index <= logic->EditLastLine) )
                {
                    visuals->Sprites[1300].ScreenX = 320-26;
                    visuals->Sprites[1300].ScreenY = codeScreenY;
                    visuals->Sprites[1300].RedHue = 100;
                    visuals->Sprites[1300].GreenHue = 200;
                    visuals->Sprites[1300].BlueHue = 100;
                    visuals->DrawSpriteOntoScreenBuffer(1300);
                }
                else if ( (logic->EditFirstLine >= logic->EditLastLine) && (index <= logic->EditFirstLine && index >= logic->EditLastLine) )
                {
                    visuals->Sprites[1300].ScreenX = 320-26;
                    visuals->Sprites[1300].ScreenY = codeScreenY;
                    visuals->Sprites[1300].RedHue = 100;
                    visuals->Sprites[1300].GreenHue = 200;
                    visuals->Sprites[1300].BlueHue = 100;
                    visuals->DrawSpriteOntoScreenBuffer(1300);
                }
            }

            if ( (logic->Codes[index].CodeCommandIndex > -1) || (logic->Codes[index].CodeCommandLineNumber > -1) )
            {
                if (logic->Codes[index].CodeCommandLineNumber > -1)
                {
                    char temp[256];
                    sprintf(temp, "%i", logic->Codes[index].CodeCommandLineNumber);
                    strcpy(visuals->VariableText, temp);
                    visuals->DrawSentenceOntoScreenBuffer(0, visuals->VariableText, 90, codeScreenY, JustifyRight, 0, 255, 255, 255, 1.2, 1.0);
                }

                if (logic->Codes[index].CodeCommandIndex > -1)
                {
                    if (logic->CodeSelectedByMouse == index)
                    {
                        visuals->DrawSentenceOntoScreenBuffer(0, visuals->Commands[ logic->Codes[index].CodeCommandIndex ].CommandTexts, 65+40, codeScreenY, JustifyLeft, 0, 255, 0, 255, 1.6, 1.6);
                    }
                    else
                    {
                        visuals->DrawSentenceOntoScreenBuffer(0, visuals->Commands[ logic->Codes[index].CodeCommandIndex ].CommandTexts, 65+40, codeScreenY, JustifyLeft, 255, 255, 255, 255, 1.6, 1.6);
                    }
                }
            }

            logic->CodeSelectedByMouseOld = logic->CodeSelectedByMouse;

            codeScreenY+=codeOffsetY;
        }

        interface->Buttons[11].ScreenY = (codeScreenY-17);
    }

    if (ScreenTransitionStatus == FadeOut && ScreenFadeTransparency == 255)
    {
        ScreenTransitionStatus = FadeAll;
        ScreenToDisplay = SDL_Screen;
    }
}
