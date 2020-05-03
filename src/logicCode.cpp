#include "SDL.h"

#include <stdio.h>

#include "logicCode.h"

#include "interface.h"
#include "input.h"
#include "visuals.h"
#include "screens.h"

extern Interface* interface;
extern Input* input;
extern Visuals* visuals;
extern Screens* screens;

//-------------------------------------------------------------------------------------------------
LogicCode::LogicCode(void)
{
    CommandBoxMaxY = 10;
    CodeBoxMaxY = 10;

    ScrollNumberMoved = 0;
    ScrollSpeed = 3;

    CommandDisplayStartIndex = 0;
    CommandDisplayEndIndex = CommandBoxMaxY;

    CommandSelectedByMouseOld = -1;
    CommandSelectedByMouse = -1;

    for (int index = 0; index < NumberOfCodes; index++)
    {
        Codes[index].CodeCommandLineNumber = -1;
        Codes[index].CodeCommandIndex = -1;
        Codes[index].CodeCommandLineActive = false;
    }

for (int index = 0; index < 10; index++)
{
    Codes[index].CodeCommandIndex = index;
    Codes[index].CodeCommandLineActive = true;
}
Codes[10].CodeCommandLineActive = true;

    CurrentCodeLine = 0;

    CodeDisplayStartIndex = 0;
    CodeDisplayEndIndex = CodeBoxMaxY;

    CodeSelectedByMouseOld = -1;
    CodeSelectedByMouse = -1;

    CodeSelectedForEdit = -1;
    CodeSelectorSelected = -1;

    CodeSelectedForLineNumberEdit = -1;

    CodeBoxOffsetY = 0;

    EditLineSelectedByMouse = -1;
    EditFirstLine = -1;
    EditLastLine = -1;
    EditLocation = -1;

    CalculateCodeLastLine();

    interface->CurrentInterfaceLevel = 0;

    DialogToShow = DialogNothing;
    DialogToShowOld = DialogToShow;

    for (int index = 0; index < 3; index++)
    {
        LineNumberArray[index] = -1;
    }
}

//-------------------------------------------------------------------------------------------------
LogicCode::~LogicCode(void)
{

}

//-------------------------------------------------------------------------------------------------
void LogicCode::CalculateCodeLastLine(void)
{
    CodeLastLine = 0;
    for (int index = 0; index < NumberOfCodes; index++)
    {
        if ( Codes[index].CodeCommandLineActive == true )
        {
            CodeLastLine = index;
        }
    }
}

//-------------------------------------------------------------------------------------------------
bool LogicCode::ThereIsCodeAfterThisLine(int line)
{
    for (int index = line; index < NumberOfCodes; index++)
    {
        if (Codes[index].CodeCommandLineActive == true)
        {
            return(true);
            break;
        }
    }

    return(false);
}

//-------------------------------------------------------------------------------------------------
void LogicCode::ShowHideCodeSelectLineNumberBoxes(void)
{
    for (int index = 0; index < CodeBoxMaxY; index++)
    {
        interface->Buttons[interface->CodeSelectorButtonsStart+index].RedHue = 255;
        interface->Buttons[interface->CodeSelectorButtonsStart+index].GreenHue = 255;
        interface->Buttons[interface->CodeSelectorButtonsStart+index].BlueHue = 255;
    }

    if (CodeSelectorSelected > -1)
    {
        interface->Buttons[interface->CodeSelectorButtonsStart+CodeSelectorSelected].RedHue = 0;
        interface->Buttons[interface->CodeSelectorButtonsStart+CodeSelectorSelected].GreenHue = 255;
        interface->Buttons[interface->CodeSelectorButtonsStart+CodeSelectorSelected].BlueHue = 0;
    }

    for (int index = 0; index < 12; index++)
    {
        interface->Buttons[interface->CodeSelectorButtonsStart+index].ScreenX = -999;
        interface->Buttons[interface->CodeLineSelectorButtonsStart+index].ScreenX = -999;
    }

    int codeEditScreenY = 109+200-2+5-10;
    int codeEditOffsetY = 32-10-4;
    for (int index = 0; index < CodeBoxMaxY; index++)
    {
        if ( (Codes[CodeDisplayStartIndex+index].CodeCommandLineActive == true || ThereIsCodeAfterThisLine(CodeDisplayStartIndex+index) == true) )
        {
            interface->Buttons[interface->CodeSelectorButtonsStart+index].ScreenX = 43;
            interface->Buttons[interface->CodeLineSelectorButtonsStart+index].ScreenX = 73;

            interface->Buttons[interface->CodeSelectorButtonsStart+index].ScreenY = codeEditScreenY + CodeBoxOffsetY;
            interface->Buttons[interface->CodeLineSelectorButtonsStart+index].ScreenY = codeEditScreenY + CodeBoxOffsetY;

            codeEditScreenY+=codeEditOffsetY;
        }
        else
        {
            interface->Buttons[interface->CodeSelectorButtonsStart+index].ScreenX = -999;
            interface->Buttons[interface->CodeLineSelectorButtonsStart+index].ScreenX = -999;
        }
    }
}

//-------------------------------------------------------------------------------------------------
bool LogicCode::ThereAreLineNumbers(void)
{
    for (int index = 0; index < NumberOfCodes; index++)
    {
        if (Codes[index].CodeCommandLineNumber > -1)
        {
            return(true);
        }
    }

    return(false);
}

//-------------------------------------------------------------------------------------------------
void LogicCode::ClearAllCodeSelectors(void)
{
    for ( int index = 12; index < (12+CodeBoxMaxY); index++ )
    {
        interface->Buttons[index].RedHue = 255;
        interface->Buttons[index].BlueHue = 255;
    }

    CodeSelectorSelected = -1;
    CodeSelectedForEdit = -1;

    ShowHideCodeSelectLineNumberBoxes();

    screens->ScreenIsDirty = true;
}

//-------------------------------------------------------------------------------------------------
void LogicCode::SetupClickableButtons(void)
{
    if ( ThereIsCodeAfterThisLine(1) == true )
    {
        interface->Buttons[2].BlockPressing = false;
    }
    else
    {
        interface->Buttons[2].BlockPressing = true;
    }

    if (CodeSelectedForEdit > -1)
    {
        interface->Buttons[4].BlockPressing = false;
        interface->Buttons[5].BlockPressing = false;
    }
    else
    {
        interface->Buttons[4].BlockPressing = true;
        interface->Buttons[5].BlockPressing = true;
    }

    if ( ThereAreLineNumbers() == false )
    {
        interface->Buttons[6].BlockPressing = true;
    }
    else
    {
        interface->Buttons[6].BlockPressing = false;
    }

    if ( ThereIsCodeAfterThisLine(1) == true )
    {
        interface->Buttons[7].BlockPressing = false;
    }
    else
    {
        interface->Buttons[7].BlockPressing = true;
    }
}

//-------------------------------------------------------------------------------------------------
void LogicCode::CheckForClearButton(void)
{
    if (interface->ThisButtonWasPressed == 2)
    {
        if (ThereIsCodeAfterThisLine(-1) == true)
        {
            interface->CurrentInterfaceLevel = 1;

            DialogToShow = DialogClearCode;
            DialogToShowOld = DialogClearCode;
        }
    }
}

//-------------------------------------------------------------------------------------------------
void LogicCode::CheckForInsertButton(void)
{
    if (interface->ThisButtonWasPressed == 4 && CodeSelectedForEdit > -1)
    {
        if (interface->Buttons[5].RedHue != 255 && interface->Buttons[5].BlueHue != 255)
        {
            for ( int index = (CodeLastLine+1); index > CodeSelectedForEdit; index-- )
            {
                Codes[index].CodeCommandIndex = Codes[index-1].CodeCommandIndex;
                Codes[index].CodeCommandLineNumber = Codes[index-1].CodeCommandLineNumber;
                Codes[index].CodeCommandLineActive = Codes[index-1].CodeCommandLineActive;
            }

            Codes[CodeSelectedForEdit].CodeCommandIndex = -1;
            Codes[CodeSelectedForEdit].CodeCommandLineNumber = -1;
            Codes[CodeSelectedForEdit].CodeCommandLineActive = true;

            CalculateCodeLastLine();

            interface->Buttons[4].RedHue = 150;
            interface->Buttons[4].BlueHue = 150;

            interface->Buttons[5].RedHue = 150;
            interface->Buttons[5].BlueHue = 150;

            ShowHideCodeSelectLineNumberBoxes();
            screens->ScreenIsDirty = true;
        }
    }
}

//-------------------------------------------------------------------------------------------------
void LogicCode::CheckForDeleteButton(void)
{
    if (interface->ThisButtonWasPressed == 5 && CodeSelectedForEdit > -1)
    {
        if (interface->Buttons[5].RedHue != 255 && interface->Buttons[5].BlueHue != 255)
        {
            if (Codes[CodeSelectedForEdit].CodeCommandLineActive == true && ThereIsCodeAfterThisLine(CodeSelectedForEdit+1) == false)
            {

            }
            else
            {
                for (int index = CodeSelectedForEdit; index < CodeLastLine; index++)
                {
                    Codes[index].CodeCommandIndex = Codes[index+1].CodeCommandIndex;
                    Codes[index].CodeCommandLineNumber = Codes[index+1].CodeCommandLineNumber;
                    Codes[index].CodeCommandLineActive = Codes[index+1].CodeCommandLineActive;
                }

                Codes[CodeLastLine].CodeCommandIndex = -1;
                Codes[CodeLastLine].CodeCommandLineNumber = -1;
                Codes[CodeLastLine].CodeCommandLineActive = false;

                CalculateCodeLastLine();

                if (CodeLastLine == 0)
                {
                    CodeSelectedForEdit = -1;
                    CodeSelectorSelected = -1;
                }

                if (CodeDisplayStartIndex > 0 && ThereIsCodeAfterThisLine(CodeSelectedForEdit+1) == false)
                {
                    CodeDisplayStartIndex--;
                    CodeDisplayEndIndex--;

                    CodeSelectedForEdit--;
                }
                else if ( ThereIsCodeAfterThisLine(CodeSelectedForEdit+1) == false )
                {
                    CodeSelectedForEdit--;
                    CodeSelectorSelected--;
                }

                ShowHideCodeSelectLineNumberBoxes();
                screens->ScreenIsDirty = true;
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------
void LogicCode::CheckForFindButton(void)
{
    if (interface->ThisButtonWasPressed == 6)
    {
        interface->CurrentInterfaceLevel = 2;

        DialogToShow = DialogFindLineNumber;
        DialogToShowOld = DialogToShow;

        LineNumberArray[0] = 0;
        LineNumberArray[1] = 0;
        LineNumberArray[2] = 0;

        screens->LineNumberFoundNew = -1;
        screens->LineNumberFoundOld = screens->LineNumberFoundNew;
    }
}

//-------------------------------------------------------------------------------------------------
void LogicCode::CheckForEditButton(void)
{
    if (interface->ThisButtonWasPressed == 7)
    {
        interface->CurrentInterfaceLevel = 2;

        for (int index = 0; index < 8; index++)
        {
            interface->Buttons[index].ScreenX = -999;
        }

        interface->Buttons[8].ScreenX = -999;
        interface->Buttons[9].ScreenX = -999;

        interface->Buttons[10].InterfaceLevel = 2;
        interface->Buttons[11].InterfaceLevel = 2;

        int buttonScreenX = 595+4;
        int buttonOffsetY = 57;
        int buttonScreenY = 40+(buttonOffsetY*4);
        interface->EditQuitButtonIndex = interface->CreateButtonWithText(2, true, true, "QUIT", 5.0, 2.0, 1200, buttonScreenX, buttonScreenY, 255, 255, 255, 255, 0.3, 1.0);
        buttonScreenY+=buttonOffsetY;
        interface->EditDelButtonIndex = interface->CreateButtonWithText(2, true, true, "DEL", 5.0, 2.0, 1200, buttonScreenX, buttonScreenY, 255, 255, 255, 255, 0.3, 1.0);
        buttonScreenY+=buttonOffsetY;
        interface->EditMoveButtonIndex = interface->CreateButtonWithText(2, true, true, "MOVE", 5.0, 2.0, 1200, buttonScreenX, buttonScreenY, 255, 255, 255, 255, 0.3, 1.0);
        buttonScreenY+=buttonOffsetY;
        interface->EditCopyButtonIndex = interface->CreateButtonWithText(2, true, true, "COPY", 5.0, 2.0, 1200, buttonScreenX, buttonScreenY, 255, 255, 255, 255, 0.3, 1.0);

        interface->EditStatus = EditStatusFirst;

        interface->Buttons[interface->EditDelButtonIndex].BlockPressing = true;
        interface->Buttons[interface->EditMoveButtonIndex].BlockPressing = true;
        interface->Buttons[interface->EditCopyButtonIndex].BlockPressing = true;

        EditLineSelectedByMouse = -1;
        EditFirstLine = -1;
        EditLastLine = -1;
        EditLocation = -1;

        DialogToShow = DialogEdit;
        DialogToShowOld = DialogToShow;

        CodeSelectorSelected = -1;
        ShowHideCodeSelectLineNumberBoxes();
    }
}

//-------------------------------------------------------------------------------------------------
void LogicCode::CheckForEditQuitButton(void)
{
    if (interface->CurrentInterfaceLevel == 2 && DialogToShow == DialogEdit)
    {
        if (interface->ThisButtonWasPressed == interface->EditQuitButtonIndex)
        {
            interface->CurrentInterfaceLevel = 0;

            for (int index = 0; index < 8; index++)
            {
                interface->Buttons[index].ScreenX = (595+4);
            }

            interface->Buttons[8].ScreenX = 16;
            interface->Buttons[9].ScreenX = 16;

            interface->Buttons[10].InterfaceLevel = 0;
            interface->Buttons[11].InterfaceLevel = 0;

            for ( int index = interface->EditQuitButtonIndex; index < (interface->EditCopyButtonIndex+1); index++ )
            {
                interface->Buttons[index].OneClick = true;
                interface->Buttons[index].SpriteIndex = -1;
                interface->Buttons[index].Text = " ";
                interface->Buttons[index].TextScaleX = 1.0;
                interface->Buttons[index].TextScaleY = 1.0;
                interface->Buttons[index].ScreenIndex = -1;
                interface->Buttons[index].ScreenX = 320;
                interface->Buttons[index].ScreenY = 240;
                interface->Buttons[index].RedHue = 255;
                interface->Buttons[index].GreenHue = 255;
                interface->Buttons[index].BlueHue = 255;
                interface->Buttons[index].Transparency = 255;
                interface->Buttons[index].ScaleX = 1.0;
                interface->Buttons[index].ScaleY = 1.0;
                interface->Buttons[index].AnimationScale = 1.0;
                interface->Buttons[index].AnimationTimer = -1;
                interface->Buttons[index].BlockPressing = false;
                interface->Buttons[index].InterfaceLevel = 0;
                interface->Buttons[index].PlaySound = true;
            }

            interface->EditStatus = EditStatusOff;

            EditLineSelectedByMouse = -1;
            EditFirstLine = -1;
            EditLastLine = -1;
            EditLocation = -1;

            DialogToShow = DialogNothing;
            DialogToShowOld = DialogToShow;
        }
    }
}

//-------------------------------------------------------------------------------------------------
void LogicCode::CheckForEditDelButton(void)
{
    if (interface->CurrentInterfaceLevel == 2 && DialogToShow == DialogEdit)
    {
        if (interface->ThisButtonWasPressed == interface->EditDelButtonIndex && interface->EditStatus < EditStatusLocationMove)
        {
            interface->CurrentInterfaceLevel = 0;

            for (int index = 0; index < 8; index++)
            {
                interface->Buttons[index].ScreenX = (595+4);
            }

            interface->Buttons[8].ScreenX = 16;
            interface->Buttons[9].ScreenX = 16;

            interface->Buttons[10].InterfaceLevel = 0;
            interface->Buttons[11].InterfaceLevel = 0;

            for ( int index = interface->EditQuitButtonIndex; index < (interface->EditCopyButtonIndex+1); index++ )
            {
                interface->Buttons[index].OneClick = true;
                interface->Buttons[index].SpriteIndex = -1;
                interface->Buttons[index].Text = " ";
                interface->Buttons[index].TextScaleX = 1.0;
                interface->Buttons[index].TextScaleY = 1.0;
                interface->Buttons[index].ScreenIndex = -1;
                interface->Buttons[index].ScreenX = 320;
                interface->Buttons[index].ScreenY = 240;
                interface->Buttons[index].RedHue = 255;
                interface->Buttons[index].GreenHue = 255;
                interface->Buttons[index].BlueHue = 255;
                interface->Buttons[index].Transparency = 255;
                interface->Buttons[index].ScaleX = 1.0;
                interface->Buttons[index].ScaleY = 1.0;
                interface->Buttons[index].AnimationScale = 1.0;
                interface->Buttons[index].AnimationTimer = -1;
                interface->Buttons[index].BlockPressing = false;
                interface->Buttons[index].InterfaceLevel = 0;
                interface->Buttons[index].PlaySound = true;
            }

            for ( int index = EditFirstLine; index < EditLastLine+1; index++)
            {
                Codes[index].CodeCommandIndex = -1;
                Codes[index].CodeCommandLineActive = false;
                Codes[index].CodeCommandLineNumber = -2;
            }

            while (Codes[EditFirstLine].CodeCommandLineNumber == -2)
            {
                for (int index = EditFirstLine; index < NumberOfCodes; index++)
                {
                    Codes[index].CodeCommandIndex = Codes[index+1].CodeCommandIndex;
                    Codes[index].CodeCommandLineActive = Codes[index+1].CodeCommandLineActive;
                    Codes[index].CodeCommandLineNumber = Codes[index+1].CodeCommandLineNumber;
                }

                Codes[NumberOfCodes-1].CodeCommandIndex = -1;
                Codes[NumberOfCodes-1].CodeCommandLineActive = false;
                Codes[NumberOfCodes-1].CodeCommandLineNumber = -1;
            }

            if (Codes[1].CodeCommandLineActive == false)
            {
                Codes[0].CodeCommandIndex = -1;
                Codes[0].CodeCommandLineActive = true;
                Codes[0].CodeCommandLineNumber = -1;

                CodeDisplayStartIndex = 0;
            }

            interface->SetupCodingWindows();
            ShowHideCodeSelectLineNumberBoxes();

            interface->EditStatus = EditStatusOff;

            EditLineSelectedByMouse = -1;
            EditFirstLine = -1;
            EditLastLine = -1;
            EditLocation = -1;

            DialogToShow = DialogNothing;
            DialogToShowOld = DialogToShow;
        }

    }

}

//-------------------------------------------------------------------------------------------------
void LogicCode::CheckForEditMoveButton(void)
{
    if (interface->CurrentInterfaceLevel == 2 && DialogToShow == DialogEdit)
    {
        if (interface->ThisButtonWasPressed == interface->EditMoveButtonIndex && interface->EditStatus == EditStatusCommand)
        {
            interface->EditStatus = EditStatusLocationMove;

            interface->Buttons[interface->EditDelButtonIndex].BlockPressing = true;
            interface->Buttons[interface->EditMoveButtonIndex].BlockPressing = true;
            interface->Buttons[interface->EditCopyButtonIndex].BlockPressing = true;
        }
    }
}

//-------------------------------------------------------------------------------------------------
void LogicCode::CheckForEditCopyButton(void)
{
    if (interface->CurrentInterfaceLevel == 2 && DialogToShow == DialogEdit)
    {
        if (interface->ThisButtonWasPressed == interface->EditCopyButtonIndex && interface->EditStatus == EditStatusCommand)
        {
            interface->EditStatus = EditStatusLocationCopy;

            interface->Buttons[interface->EditDelButtonIndex].BlockPressing = true;
            interface->Buttons[interface->EditMoveButtonIndex].BlockPressing = true;
            interface->Buttons[interface->EditCopyButtonIndex].BlockPressing = true;
        }
    }
}

//-------------------------------------------------------------------------------------------------
void LogicCode::CheckForScrollArrowButtons(void)
{
    if (interface->CurrentInterfaceLevel == 0)
    {
        if (input->MouseWheelStatus == MouseWheelUp)
        {
            if (  ( input->MouseY > (91) )
               && ( input->MouseY < (260 + CodeBoxOffsetY) )
               && ( input->MouseX > (32) )
               && ( input->MouseX < (554) )  )
            {
                interface->ThisButtonWasPressed = 8;
                screens->ScreenIsDirty = true;
            }
        }
        else if (input->MouseWheelStatus == MouseWheelDown)
        {
            if (  ( input->MouseY > (91) )
               && ( input->MouseY < (260 + CodeBoxOffsetY) )
               && ( input->MouseX > (32) )
               && ( input->MouseX < (554) )  )
            {
                interface->ThisButtonWasPressed = 9;
                screens->ScreenIsDirty = true;
            }
        }
    }

    if (interface->ThisButtonWasPressed == 8)
    {
        if (input->MouseWheelStatus == MouseWheelUp)
        {
            for (int index = 0; index < 3; index++)
            {
                if (CommandDisplayStartIndex > 0)
                {
                    CommandDisplayStartIndex--;
                    CommandDisplayEndIndex--;
                }
            }
        }
        else if (CommandDisplayStartIndex > 0)
        {
            CommandDisplayStartIndex--;
            CommandDisplayEndIndex--;
        }
    }
    else if (interface->ThisButtonWasPressed == 9)
    {
        if (input->MouseWheelStatus == MouseWheelDown)
        {
            for (int index = 0; index < 3; index++)
            {
                if ( CommandDisplayEndIndex < (103) )
                {
                    CommandDisplayStartIndex++;
                    CommandDisplayEndIndex++;
                }
            }
        }
        else if ( CommandDisplayStartIndex < (103-CommandBoxMaxY) )
        {
            CommandDisplayStartIndex++;
            CommandDisplayEndIndex++;
        }
    }

    if ( (interface->CurrentInterfaceLevel == 0 && interface->EditorResizeButtonOriginalPressY == -1) )
    {
        CommandSelectedByMouse = -1;
        int commandScreenY = 107-10;
        int commandOffsetY = 32-10-4;
        for (int index = 0; index < CommandBoxMaxY; index++)
        {
            if (  ( input->MouseY > (commandScreenY-10) )
               && ( input->MouseY < (commandScreenY+10) )
               && ( input->MouseX > (320-270) )
               && ( input->MouseX < (320+200) )  )
            {
                CommandSelectedByMouse = (CommandDisplayStartIndex+index);
            }

            commandScreenY+=commandOffsetY;
        }
    }

    if ( (interface->CurrentInterfaceLevel == 0 && interface->EditorResizeButtonOriginalPressY == -1) || (interface->CurrentInterfaceLevel == 2 && interface->EditStatus != EditStatusOff) )
    {
        CodeSelectedByMouse = -1;
        EditLineSelectedByMouse = -1;
        int codeScreenY = 109+200-2+CodeBoxOffsetY + 5 - 10;
        int codeOffsetY = 32-10-4;
        for (int index = 0; index < CodeBoxMaxY; index++)
        {
            if (  ( input->MouseY > (codeScreenY-10) )
               && ( input->MouseY < (codeScreenY+10) )
               && ( input->MouseX > (320-220) )
               && ( input->MouseX < (320+200) )  )
            {
                CodeSelectedByMouse = (CodeDisplayStartIndex+index);
                EditLineSelectedByMouse = (CodeDisplayStartIndex+index);

                if (input->MouseButtonPressed[0] == true)
                {
                    if (interface->CurrentInterfaceLevel == 2 && interface->EditStatus == EditStatusFirst)
                    {
                        EditFirstLine = (CodeDisplayStartIndex+index);
                        interface->EditStatus = EditStatusLast;

                        screens->ScreenIsDirty = true;
                    }
                    else if (interface->CurrentInterfaceLevel == 2 && interface->EditStatus == EditStatusLast && Codes[CodeDisplayStartIndex+index+1].CodeCommandLineActive == true)
                    {
                        EditLastLine = (CodeDisplayStartIndex+index);
                        interface->EditStatus = EditStatusCommand;

                        if (EditLastLine < EditFirstLine)
                        {
                            int temp = EditFirstLine;
                            EditFirstLine = EditLastLine;
                            EditLastLine = temp;
                        }

                        interface->Buttons[interface->EditDelButtonIndex].BlockPressing = false;
                        interface->Buttons[interface->EditMoveButtonIndex].BlockPressing = false;
                        interface->Buttons[interface->EditCopyButtonIndex].BlockPressing = false;

                        screens->ScreenIsDirty = true;
                    }
                    else if (interface->CurrentInterfaceLevel == 2 && interface->EditStatus >= EditStatusLocationMove)
                    {
                        EditLocation = (CodeDisplayStartIndex+index);
                        if ( (EditLocation >= EditFirstLine && EditLocation <= EditLastLine) || (Codes[EditLocation].CodeCommandLineActive == false) )
                        {
                            EditLocation = -1;
                        }

                        if (EditLocation > -1)
                        {
                            for (int index = 0; index < NumberOfCodes; index++)
                            {
                                CacheCodes[index].CodeCommandIndex = -1;
                                CacheCodes[index].CodeCommandLineActive = false;
                                CacheCodes[index].CodeCommandLineNumber = -1;
                            }

                            int cacheIndex = 0;
                            for ( int index = EditFirstLine; index < (EditLastLine+1); index++)
                            {
                                CacheCodes[cacheIndex].CodeCommandIndex = Codes[index].CodeCommandIndex;
                                CacheCodes[cacheIndex].CodeCommandLineActive = Codes[index].CodeCommandLineActive;
                                CacheCodes[cacheIndex].CodeCommandLineNumber = Codes[index].CodeCommandLineNumber;

                                cacheIndex++;
                            }

                            int sizeOfLines = (EditLastLine-EditFirstLine);

                            if (interface->EditStatus == EditStatusLocationMove)
                            {
                                for ( int index = EditFirstLine; index < (EditLastLine+1); index++)
                                {
                                    Codes[index].CodeCommandIndex = -1;
                                    Codes[index].CodeCommandLineActive = false;
                                    Codes[index].CodeCommandLineNumber = -2;
                                }

                                while (Codes[EditFirstLine].CodeCommandLineNumber == -2)
                                {
                                    for (int index = EditFirstLine; index < NumberOfCodes; index++)
                                    {
                                        Codes[index].CodeCommandIndex = Codes[index+1].CodeCommandIndex;
                                        Codes[index].CodeCommandLineActive = Codes[index+1].CodeCommandLineActive;
                                        Codes[index].CodeCommandLineNumber = Codes[index+1].CodeCommandLineNumber;
                                    }

                                    Codes[NumberOfCodes-1].CodeCommandIndex = -1;
                                    Codes[NumberOfCodes-1].CodeCommandLineActive = false;
                                    Codes[NumberOfCodes-1].CodeCommandLineNumber = -1;
                                }

                                if (Codes[0].CodeCommandLineActive == false)
                                {
                                    Codes[0].CodeCommandIndex = -1;
                                    Codes[0].CodeCommandLineActive = true;
                                    Codes[0].CodeCommandLineNumber = -1;

                                    CodeDisplayStartIndex = 0;
                                }

                                if (EditLocation > EditLastLine)
                                {
                                    EditLocation-=(sizeOfLines);
                                }

                                for ( int cacheIndex = 0; cacheIndex < (sizeOfLines+1); cacheIndex++ )
                                {
                                    for ( int index = (NumberOfCodes-1); index > (EditLocation+cacheIndex); index-- )
                                    {
                                        Codes[index].CodeCommandIndex = Codes[index-1].CodeCommandIndex;
                                        Codes[index].CodeCommandLineActive = Codes[index-1].CodeCommandLineActive;
                                        Codes[index].CodeCommandLineNumber = Codes[index-1].CodeCommandLineNumber;
                                    }

                                    Codes[EditLocation+cacheIndex].CodeCommandIndex = CacheCodes[cacheIndex].CodeCommandIndex;
                                    Codes[EditLocation+cacheIndex].CodeCommandLineActive = true;
                                    Codes[EditLocation+cacheIndex].CodeCommandLineNumber = -1;
                                }

                                if ( ThereIsCodeAfterThisLine(EditLocation+sizeOfLines+1) == false )
                                {
                                    Codes[EditLocation+sizeOfLines+1].CodeCommandIndex = -1;
                                    Codes[EditLocation+sizeOfLines+1].CodeCommandLineActive = true;
                                    Codes[EditLocation+sizeOfLines+1].CodeCommandLineNumber = -1;
                                }
                            }
                            else if (interface->EditStatus == EditStatusLocationCopy)
                            {
                                for ( int cacheIndex = 0; cacheIndex < (sizeOfLines+1); cacheIndex++ )
                                {
                                    for ( int index = (NumberOfCodes-1); index > (EditLocation+cacheIndex); index-- )
                                    {
                                        Codes[index].CodeCommandIndex = Codes[index-1].CodeCommandIndex;
                                        Codes[index].CodeCommandLineActive = Codes[index-1].CodeCommandLineActive;
                                        Codes[index].CodeCommandLineNumber = Codes[index-1].CodeCommandLineNumber;
                                    }

                                    Codes[EditLocation+cacheIndex].CodeCommandIndex = CacheCodes[cacheIndex].CodeCommandIndex;
                                    Codes[EditLocation+cacheIndex].CodeCommandLineActive = true;
                                    Codes[EditLocation+cacheIndex].CodeCommandLineNumber = -1;
                                }

                                if ( ThereIsCodeAfterThisLine(EditLocation+sizeOfLines+1) == false )
                                {
                                    Codes[EditLocation+sizeOfLines+1].CodeCommandIndex = -1;
                                    Codes[EditLocation+sizeOfLines+1].CodeCommandLineActive = true;
                                    Codes[EditLocation+sizeOfLines+1].CodeCommandLineNumber = -1;
                                }
                            }

                            interface->CurrentInterfaceLevel = 0;

                            for (int index = 0; index < 8; index++)
                            {
                                interface->Buttons[index].ScreenX = (595+4);
                            }

                            interface->Buttons[8].ScreenX = 16;
                            interface->Buttons[9].ScreenX = 16;

                            interface->Buttons[10].InterfaceLevel = 0;
                            interface->Buttons[11].InterfaceLevel = 0;

                            for ( int index = interface->EditQuitButtonIndex; index < (interface->EditCopyButtonIndex+1); index++ )
                            {
                                interface->Buttons[index].OneClick = true;
                                interface->Buttons[index].SpriteIndex = -1;
                                interface->Buttons[index].Text = " ";
                                interface->Buttons[index].TextScaleX = 1.0;
                                interface->Buttons[index].TextScaleY = 1.0;
                                interface->Buttons[index].ScreenIndex = -1;
                                interface->Buttons[index].ScreenX = 320;
                                interface->Buttons[index].ScreenY = 240;
                                interface->Buttons[index].RedHue = 255;
                                interface->Buttons[index].GreenHue = 255;
                                interface->Buttons[index].BlueHue = 255;
                                interface->Buttons[index].Transparency = 255;
                                interface->Buttons[index].ScaleX = 1.0;
                                interface->Buttons[index].ScaleY = 1.0;
                                interface->Buttons[index].AnimationScale = 1.0;
                                interface->Buttons[index].AnimationTimer = -1;
                                interface->Buttons[index].BlockPressing = false;
                                interface->Buttons[index].InterfaceLevel = 0;
                                interface->Buttons[index].PlaySound = true;
                            }

                            interface->SetupCodingWindows();
                            ShowHideCodeSelectLineNumberBoxes();

                            interface->EditStatus = EditStatusOff;

                            EditLineSelectedByMouse = -1;
                            EditFirstLine = -1;
                            EditLastLine = -1;
                            EditLocation = -1;

                            DialogToShow = DialogNothing;
                            DialogToShowOld = DialogToShow;

                            screens->ScreenIsDirty = true;
                        }
                    }
                }
            }

            codeScreenY+=codeOffsetY;
        }
    }

    if ( (interface->CurrentInterfaceLevel == 0) || (interface->CurrentInterfaceLevel == 2 && interface->EditStatus != EditStatusOff) )
    {
        if (input->MouseWheelStatus == MouseWheelUp)
        {
            if (  ( input->MouseY > (291 + CodeBoxOffsetY) )
               && ( input->MouseY < (460) )
               && ( input->MouseX > (32) )
               && ( input->MouseX < (554) )  )
            {
                interface->ThisButtonWasPressed = 10;
                screens->ScreenIsDirty = true;
            }
        }
        else if (input->MouseWheelStatus == MouseWheelDown)
        {
            if (  ( input->MouseY > (291 + CodeBoxOffsetY) )
               && ( input->MouseY < (460) )
               && ( input->MouseX > (32) )
               && ( input->MouseX < (554) )  )
            {
                interface->ThisButtonWasPressed = 11;
                screens->ScreenIsDirty = true;
            }
        }
    }

    if (interface->ThisButtonWasPressed == 10)
    {
        if (input->MouseWheelStatus == MouseWheelUp)
        {
            for (int index = 0; index < 3; index++)
            {
                if (CodeDisplayStartIndex > 0)
                {
                    CodeDisplayStartIndex--;
                    CodeDisplayEndIndex--;
                }
            }
        }
        else if (CodeDisplayStartIndex > 0)
        {
            CodeDisplayStartIndex--;
            CodeDisplayEndIndex--;
        }

        for (int indexTwo = 12; indexTwo < 24; indexTwo++)
        {
            interface->Buttons[indexTwo].RedHue = 255;
            interface->Buttons[indexTwo].BlueHue = 255;
        }

        CodeSelectedForEdit = -1;
        CodeSelectorSelected = -1;

        ShowHideCodeSelectLineNumberBoxes();
    }
    else if (interface->ThisButtonWasPressed == 11 && CodeDisplayEndIndex < (NumberOfCodes-5) )
    {
        if (input->MouseWheelStatus == MouseWheelDown)
        {
            for (int index = 0; index < 3; index++)
            {
                if ( CodeDisplayEndIndex < (CodeLastLine+CodeBoxMaxY-1) )
                {
                    CodeDisplayStartIndex++;
                    CodeDisplayEndIndex++;
                }
            }
        }
        else if ( CodeDisplayStartIndex < (CodeLastLine-1) )
        {
            CodeDisplayStartIndex++;
            CodeDisplayEndIndex++;
        }

        for (int indexTwo = 12; indexTwo < 24; indexTwo++)
        {
            interface->Buttons[indexTwo].RedHue = 255;
            interface->Buttons[indexTwo].BlueHue = 255;
        }

        CodeSelectedForEdit = -1;
        CodeSelectorSelected = -1;

        ShowHideCodeSelectLineNumberBoxes();
    }
}

//-------------------------------------------------------------------------------------------------
void LogicCode::CheckForCodeSelectButtons(void)
{
    for ( int index = interface->CodeSelectorButtonsStart; index < (interface->CodeSelectorButtonsStart+CodeBoxMaxY); index++ )
    {
        if (interface->ThisButtonWasPressed == index)
        {
            if ( CodeSelectedForEdit != CodeDisplayStartIndex+(index-interface->CodeSelectorButtonsStart) )
            {
                CodeSelectorSelected = (index-interface->CodeSelectorButtonsStart);
                CodeSelectedForEdit = CodeDisplayStartIndex+(index-interface->CodeSelectorButtonsStart);

                screens->ScreenIsDirty = true;
            }
            else
            {
                interface->Buttons[index].RedHue = 255;
                interface->Buttons[index].BlueHue = 255;

                CodeSelectorSelected = -1;
                CodeSelectedForEdit = -1;

                screens->ScreenIsDirty = true;
            }

            ShowHideCodeSelectLineNumberBoxes();
        }
    }

    if (CodeSelectedForEdit > -1)
    {
        interface->Buttons[4].RedHue = 150;
        interface->Buttons[4].BlueHue = 150;

        interface->Buttons[5].RedHue = 150;
        interface->Buttons[5].BlueHue = 150;
    }
    else if (CodeSelectedForEdit == -1)
    {
        interface->Buttons[4].RedHue = 255;
        interface->Buttons[4].BlueHue = 255;

        interface->Buttons[5].RedHue = 255;
        interface->Buttons[5].BlueHue = 255;
    }
}

//-------------------------------------------------------------------------------------------------
void LogicCode::CheckForCodeLineSelectButtons(void)
{
    for ( int index = interface->CodeLineSelectorButtonsStart; index < (interface->CodeLineSelectorButtonsStart+CodeBoxMaxY); index++ )
    {
        if (  interface->ThisButtonWasPressed == index && ThereIsCodeAfterThisLine( CodeDisplayStartIndex+(index-interface->CodeLineSelectorButtonsStart) + 1 ) == true  )
        {
            CodeSelectedForLineNumberEdit = CodeDisplayStartIndex+(index-interface->CodeLineSelectorButtonsStart);

            interface->CurrentInterfaceLevel = 1;

            DialogToShow = DialogLineNumberSelect;
            DialogToShowOld = DialogToShow;

            LineNumberArray[0] = 0;
            LineNumberArray[1] = 0;
            LineNumberArray[2] = 0;
            for ( int number = 0; number < Codes[CodeDisplayStartIndex+CodeSelectedForLineNumberEdit].CodeCommandLineNumber; number++ )
            {
                LineNumberArray[2]++;
                if (LineNumberArray[2] > 9)
                {
                    LineNumberArray[2] = 0;
                    LineNumberArray[1]++;
                    if (LineNumberArray[1] > 9)
                    {
                        LineNumberArray[1] = 0;
                        LineNumberArray[0]++;
                    }
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------
void LogicCode::RunCodeEditor(void)
{
    SetupClickableButtons();

    CheckForClearButton();

    CheckForInsertButton();

    CheckForDeleteButton();

    CheckForFindButton();

    CheckForEditButton();
    CheckForEditQuitButton();
    CheckForEditDelButton();
    CheckForEditMoveButton();
    CheckForEditCopyButton();

    CheckForScrollArrowButtons();

    CheckForCodeSelectButtons();

    CheckForCodeLineSelectButtons();

}
