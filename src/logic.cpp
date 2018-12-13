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
    ScrollNumberMoved = 0;
    ScrollSpeed = 3;

    CommandDisplayStartIndex = 0;
    CommandDisplayEndIndex = 5;

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
    CodeDisplayEndIndex = 5;

    CodeSelectedByMouseOld = -1;
    CodeSelectedByMouse = -1;

    CodeSelectedForEdit = -1;
    CodeSelectorSelected = -1;

    CodeSelectedForLineNumberEdit = -1;

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
    for (int index = 0; index < 65000; index++)
    {
        if ( Codes[index].CodeCommandLineActive == true )// Codes[index].CodeCommandIndex == -1 && ThereIsCodeAfterThisLine(index+1) == false )
        {
            CodeLastLine = index;
//            break;
        }
    }
}

//-------------------------------------------------------------------------------------------------
bool Logic::ThereIsCodeAfterThisLine(int line)
{
    for (int index = line; index < 65000; index++)
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
    for (int index = 0; index < 5; index++)
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

    if ( (Codes[CodeDisplayStartIndex].CodeCommandLineActive == true || ThereIsCodeAfterThisLine(CodeDisplayStartIndex) == true) )
    {
        interface->Buttons[12].ScreenX = 43;
        interface->Buttons[17].ScreenX = 73;
    }
    else
    {
        interface->Buttons[12].ScreenX = -999;
        interface->Buttons[17].ScreenX = -999;
    }

    if ( (Codes[CodeDisplayStartIndex+1].CodeCommandLineActive == true || ThereIsCodeAfterThisLine(CodeDisplayStartIndex+1) == true) )
    {
        interface->Buttons[12+1].ScreenX = 43;
        interface->Buttons[17+1].ScreenX = 73;
    }
    else
    {
        interface->Buttons[12+1].ScreenX = -999;
        interface->Buttons[17+1].ScreenX = -999;
    }

    if ( (Codes[CodeDisplayStartIndex+2].CodeCommandLineActive == true || ThereIsCodeAfterThisLine(CodeDisplayStartIndex+2) == true) )
    {
        interface->Buttons[12+2].ScreenX = 43;
        interface->Buttons[17+2].ScreenX = 73;
    }
    else
    {
        interface->Buttons[12+2].ScreenX = -999;
        interface->Buttons[17+2].ScreenX = -999;
    }

    if ( (Codes[CodeDisplayStartIndex+3].CodeCommandLineActive == true || ThereIsCodeAfterThisLine(CodeDisplayStartIndex+3) == true) )
    {
        interface->Buttons[12+3].ScreenX = 43;
        interface->Buttons[17+3].ScreenX = 73;
    }
    else
    {
        interface->Buttons[12+3].ScreenX = -999;
        interface->Buttons[17+3].ScreenX = -999;
    }

    if ( (Codes[CodeDisplayStartIndex+4].CodeCommandLineActive == true || ThereIsCodeAfterThisLine(CodeDisplayStartIndex+4) == true) )
    {
        interface->Buttons[12+4].ScreenX = 43;
        interface->Buttons[17+4].ScreenX = 73;
    }
    else
    {
        interface->Buttons[12+4].ScreenX = -999;
        interface->Buttons[17+4].ScreenX = -999;
    }
}

//-------------------------------------------------------------------------------------------------
void Logic::RunCodeEditor(void)
{
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

    if (interface->ThisButtonWasPressed == 2)
    {
        if (ThereIsCodeAfterThisLine(-1) == true)
        {
            interface->CurrentInterfaceLevel = 1;

            DialogToShow = ClearCode;
            DialogToShowOld = ClearCode;
        }
    }

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

//            if (CodeLastLine < 65000)  CodeLastLine++;
            CalculateCodeLastLine();

            interface->Buttons[4].RedHue = 150;
            interface->Buttons[4].BlueHue = 150;

            interface->Buttons[5].RedHue = 150;
            interface->Buttons[5].BlueHue = 150;

            ShowHideCodeSelectLineNumberBoxes();
            screens->ScreenIsDirty = true;
        }
    }

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
    //Normal
    //visuals->Sprites[100].GreenHue = 255;
    //visuals->Sprites[100].BlueHue = 255;

                if (CodeDisplayStartIndex > 0 && ThereIsCodeAfterThisLine(CodeSelectedForEdit+1) == false)
                {
                    CodeDisplayStartIndex--;
                    CodeDisplayEndIndex--;

                    CodeSelectedForEdit--;
    //Purple
    //visuals->Sprites[100].GreenHue = 0;
    //visuals->Sprites[100].BlueHue = 255;
                }
                else if ( ThereIsCodeAfterThisLine(CodeSelectedForEdit+1) == false )
                {
                    CodeSelectedForEdit--;
                    CodeSelectorSelected--;
    //Red
    //visuals->Sprites[100].GreenHue = 0;
    //visuals->Sprites[100].BlueHue = 0;
                }
                else
                {
    //Yellow
    //visuals->Sprites[100].GreenHue = 255;
    //visuals->Sprites[100].BlueHue = 0;
                }

                ShowHideCodeSelectLineNumberBoxes();
                screens->ScreenIsDirty = true;
            }
        }
    }

    if (interface->CurrentInterfaceLevel == 0)
    {
        if (input->MouseWheelStatus == MouseWheelUp)
        {
            if (  ( input->MouseY > (91) )
               && ( input->MouseY < (260) )
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
               && ( input->MouseY < (260) )
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
                if ( CommandDisplayEndIndex < (98+5) )
                {
                    CommandDisplayStartIndex++;
                    CommandDisplayEndIndex++;
                }
            }
        }
        else if (CommandDisplayStartIndex < 98)
        {
            CommandDisplayStartIndex++;
            CommandDisplayEndIndex++;
        }
    }

    if (interface->CurrentInterfaceLevel == 0)
    {
        CommandSelectedByMouse = -1;
        int commandScreenY = 109;
        int commandOffsetY = 32;
        for (int index = 0; index < 5; index++)
        {
            if (  ( input->MouseY > (commandScreenY-20) )
               && ( input->MouseY < (commandScreenY+20) )
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
            if (  ( input->MouseY > (291) )
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
            if (  ( input->MouseY > (291) )
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

        for (int indexTwo = 12; indexTwo < 17; indexTwo++)
        {
            interface->Buttons[indexTwo].RedHue = 255;
            interface->Buttons[indexTwo].BlueHue = 255;
        }

        CodeSelectedForEdit = -1;
        CodeSelectorSelected = -1;

        ShowHideCodeSelectLineNumberBoxes();
    }
    else if (interface->ThisButtonWasPressed == 11 && CodeDisplayEndIndex < (65000-5) )
    {
        if (input->MouseWheelStatus == MouseWheelDown)
        {
            for (int index = 0; index < 3; index++)
            {
                if ( CodeDisplayEndIndex < (CodeLastLine+4) )
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

        for (int indexTwo = 12; indexTwo < 17; indexTwo++)
        {
            interface->Buttons[indexTwo].RedHue = 255;
            interface->Buttons[indexTwo].BlueHue = 255;
        }

        CodeSelectedForEdit = -1;
        CodeSelectorSelected = -1;

        ShowHideCodeSelectLineNumberBoxes();
    }

    for (int index = 12; index < 17; index++)
    {
        if (interface->ThisButtonWasPressed == index)
        {
            if ( CodeSelectedForEdit != CodeDisplayStartIndex+(index-12) )
            {
//                for (int indexTwo = 12; indexTwo < 17; indexTwo++)
//                {
//                    interface->Buttons[indexTwo].RedHue = 255;
//                    interface->Buttons[indexTwo].BlueHue = 255;
//                }
//
//                interface->Buttons[index].RedHue = 0;
//                interface->Buttons[index].BlueHue = 0;

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

    for (int index = 17; index < 22; index++)
    {
        if (interface->ThisButtonWasPressed == index)
        {
            CodeSelectedForLineNumberEdit = CodeDisplayStartIndex+(index-17);

            interface->CurrentInterfaceLevel = 1;

            DialogToShow = LineNumberSelect;
            DialogToShowOld = DialogToShow;

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
