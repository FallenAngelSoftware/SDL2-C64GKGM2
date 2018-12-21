#include "SDL.h"

#include "logic.h"

#include "interface.h"
#include "input.h"
#include "visuals.h"
#include "screens.h"

extern Interface* interface;
extern Input* input;
extern Visuals* visuals;
extern Screens* screens;

//-------------------------------------------------------------------------------------------------
Logic::Logic(void)
{

    CommandBoxMaxY = 7;
    CodeBoxMaxY = 7;

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

    CalculateCodeLastLine();

    interface->CurrentInterfaceLevel = 0;

    DialogToShow = Nothing;
    DialogToShowOld = DialogToShow;

    for (int index = 0; index < 3; index++)
    {
        LineNumberArray[index] = -1;
    }
}

//-------------------------------------------------------------------------------------------------
Logic::~Logic(void)
{

}

//-------------------------------------------------------------------------------------------------
void Logic::CalculateCodeLastLine(void)
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
bool Logic::ThereIsCodeAfterThisLine(int line)
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
void Logic::ShowHideCodeSelectLineNumberBoxes(void)
{
    for (int index = 0; index < CodeBoxMaxY; index++)
    {
        interface->Buttons[12+index].RedHue = 255;
        interface->Buttons[12+index].GreenHue = 255;
        interface->Buttons[12+index].BlueHue = 255;
    }

    if (CodeSelectorSelected > -1)
    {
        interface->Buttons[12+CodeSelectorSelected].RedHue = 0;
        interface->Buttons[12+CodeSelectorSelected].GreenHue = 255;
        interface->Buttons[12+CodeSelectorSelected].BlueHue = 0;
    }

    for (int index = 0; index < 12; index++)
    {
        interface->Buttons[12+index].ScreenX = -999;
        interface->Buttons[24+index].ScreenX = -999;
    }

    int codeEditScreenY = 109+200-2+5;
    int codeEditOffsetY = 32-10;
    for (int index = 0; index < CodeBoxMaxY; index++)
    {
        if ( (Codes[CodeDisplayStartIndex+index].CodeCommandLineActive == true || ThereIsCodeAfterThisLine(CodeDisplayStartIndex+index) == true) )
        {
            interface->Buttons[12+index].ScreenX = 43;
            interface->Buttons[24+index].ScreenX = 73;

            interface->Buttons[12+index].ScreenY = codeEditScreenY + CodeBoxOffsetY;
            interface->Buttons[24+index].ScreenY = codeEditScreenY + CodeBoxOffsetY;

            codeEditScreenY+=codeEditOffsetY;
        }
        else
        {
            interface->Buttons[12+index].ScreenX = -999;
            interface->Buttons[24+index].ScreenX = -999;
        }
    }
}

//-------------------------------------------------------------------------------------------------
bool Logic::ThereAreLineNumbers(void)
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
void Logic::SetupClickableButtons(void)
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
}

//-------------------------------------------------------------------------------------------------
void Logic::CheckForClearButton(void)
{
    if (interface->ThisButtonWasPressed == 2)
    {
        if (ThereIsCodeAfterThisLine(-1) == true)
        {
            interface->CurrentInterfaceLevel = 1;

            DialogToShow = ClearCode;
            DialogToShowOld = ClearCode;
        }
    }
}

//-------------------------------------------------------------------------------------------------
void Logic::CheckForInsertButton(void)
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
void Logic::CheckForDeleteButton(void)
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
void Logic::CheckForScrollArrowButtons(void)
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

    if (interface->CurrentInterfaceLevel == 0 && interface->EditorResizeButtonOriginalPressY == -1)
    {
        CommandSelectedByMouse = -1;
        int commandScreenY = 107;
        int commandOffsetY = 32-10;
        for (int index = 0; index < CommandBoxMaxY; index++)
        {
            if (  ( input->MouseY > (commandScreenY-17) )
               && ( input->MouseY < (commandScreenY+17) )
               && ( input->MouseX > (320-270) )
               && ( input->MouseX < (320+200) )  )
            {
                CommandSelectedByMouse = (CommandDisplayStartIndex+index);
            }

            commandScreenY+=commandOffsetY;
        }
    }

    if (interface->CurrentInterfaceLevel == 0)
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
void Logic::CheckForCodeSelectButtons(void)
{
    for ( int index = 12; index < (12+CodeBoxMaxY); index++ )
    {
        if (interface->ThisButtonWasPressed == index)
        {
            if ( CodeSelectedForEdit != CodeDisplayStartIndex+(index-12) )
            {
                CodeSelectorSelected = (index-12);
                CodeSelectedForEdit = CodeDisplayStartIndex+(index-12);

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
}

//-------------------------------------------------------------------------------------------------
void Logic::CheckForCodeLineSelectButtons(void)
{
    for ( int index = 24; index < (24+CodeBoxMaxY); index++ )
    {
        if (  interface->ThisButtonWasPressed == index && ThereIsCodeAfterThisLine( CodeDisplayStartIndex+(index-24) + 1 ) == true  )
        {
            CodeSelectedForLineNumberEdit = CodeDisplayStartIndex+(index-24);

            interface->CurrentInterfaceLevel = 1;

            DialogToShow = LineNumberSelect;
            DialogToShowOld = DialogToShow;

            LineNumberArray[0] = 0;
            LineNumberArray[1] = 0;
            LineNumberArray[2] = 0;
            for ( int number = 0; number < Codes[CommandDisplayStartIndex+CodeSelectedForLineNumberEdit].CodeCommandLineNumber; number++ )
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
void Logic::RunCodeEditor(void)
{
    CommandDisplayEndIndex = (CommandDisplayStartIndex+CommandBoxMaxY);
    CodeDisplayEndIndex = (CodeDisplayStartIndex+CodeBoxMaxY);

    SetupClickableButtons();

    CheckForClearButton();

    CheckForInsertButton();

    CheckForDeleteButton();

    CheckForScrollArrowButtons();

    CheckForCodeSelectButtons();

    CheckForCodeLineSelectButtons();

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
