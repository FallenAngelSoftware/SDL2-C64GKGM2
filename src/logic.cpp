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
        Codes[index].CodeCommandIndex = -1;

    }

Codes[0].CodeCommandIndex = 0;
Codes[1].CodeCommandIndex = 1;
Codes[2].CodeCommandIndex = 2;
Codes[3].CodeCommandIndex = 3;
Codes[4].CodeCommandIndex = 4;
Codes[5].CodeCommandIndex = 5;
Codes[6].CodeCommandIndex = 6;
Codes[7].CodeCommandIndex = 7;
Codes[8].CodeCommandIndex = 8;
Codes[9].CodeCommandIndex = 9;

    CurrentCodeLine = 0;

    CodeDisplayStartIndex = 0;
    CodeDisplayEndIndex = 5;

    CodeSelectedByMouseOld = -1;
    CodeSelectedByMouse = -1;

    CodeSelectedForEdit = -1;
    CodeSelectorSelected = -1;

    CalculateCodeLastLine();

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
        if (Codes[index].CodeCommandIndex == -1)
        {
            CodeLastLine = index;
            break;
        }
    }
}

//-------------------------------------------------------------------------------------------------
void Logic::ShowHideCodeSelectBoxes(void)
{
    if (Codes[CodeDisplayStartIndex].CodeCommandIndex > -1)
    {
        interface->Buttons[12].ScreenX = 43;
    }
    else  interface->Buttons[12].ScreenX = -999;

    if (Codes[CodeDisplayStartIndex+1].CodeCommandIndex > -1)
    {
        interface->Buttons[12+1].ScreenX = 43;
    }
    else  interface->Buttons[12+1].ScreenX = -999;

    if (Codes[CodeDisplayStartIndex+2].CodeCommandIndex > -1)
    {
        interface->Buttons[12+2].ScreenX = 43;
    }
    else  interface->Buttons[12+2].ScreenX = -999;

    if (Codes[CodeDisplayStartIndex+3].CodeCommandIndex > -1)
    {
        interface->Buttons[12+3].ScreenX = 43;
    }
    else  interface->Buttons[12+3].ScreenX = -999;

    if (Codes[CodeDisplayStartIndex+4].CodeCommandIndex > -1)
    {
        interface->Buttons[12+4].ScreenX = 43;
    }
    else  interface->Buttons[12+4].ScreenX = -999;
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

    if (interface->ThisButtonWasPressed == 5)
    {
        if (interface->Buttons[5].RedHue != 255 && interface->Buttons[5].BlueHue != 255)
        {
            Codes[CodeSelectedForEdit].CodeCommandIndex = -1;

            for (int index = CodeSelectedForEdit; index < CodeLastLine; index++)
            {
                Codes[index].CodeCommandIndex = Codes[index+1].CodeCommandIndex;
            }

            if (CodeLastLine > 0)  CodeLastLine--;

            if (Codes[CodeSelectedForEdit].CodeCommandIndex == -1 && CodeSelectedForEdit > 0)
            {
                if (CodeSelectorSelected > 0)
                {
                    for (int index = 12; index < 17; index++)
                    {
                        interface->Buttons[index].RedHue = 255;
                        interface->Buttons[index].BlueHue = 255;
                    }

                    CodeSelectedForEdit--;
                    CodeSelectorSelected--;

                    interface->Buttons[CodeSelectorSelected+12].RedHue = 0;
                    interface->Buttons[CodeSelectorSelected+12].BlueHue = 0;
                }
            }

            if (CodeLastLine == 0)  CodeSelectedForEdit = -1;

            Codes[CodeLastLine].CodeCommandIndex = -1;

            if (CodeDisplayStartIndex > 0)
            {
                CodeDisplayStartIndex--;
                CodeDisplayEndIndex--;
            }

            ShowHideCodeSelectBoxes();
            screens->ScreenIsDirty = true;
        }
    }

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

        ShowHideCodeSelectBoxes();
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

        ShowHideCodeSelectBoxes();
    }

    for (int index = 12; index < 17; index++)
    {
        if (interface->ThisButtonWasPressed == index)
        {
            if ( CodeSelectedForEdit != CodeDisplayStartIndex+(index-12) )
            {
                for (int indexTwo = 12; indexTwo < 17; indexTwo++)
                {
                    interface->Buttons[indexTwo].RedHue = 255;
                    interface->Buttons[indexTwo].BlueHue = 255;
                }

                interface->Buttons[index].RedHue = 0;
                interface->Buttons[index].BlueHue = 0;

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
