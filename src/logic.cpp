#include "SDL.h"

#include "logic.h"

#include "interface.h"
#include "input.h"

extern Interface* interface;
extern Input* input;

//-------------------------------------------------------------------------------------------------
Logic::Logic(void)
{
    CommandScrollNumberMoved = 0;
    CommandScrollSpeed = 3;

    CommandDisplayStartIndex = 0;
    CommandDisplayEndIndex = 5;

    CommandSelectedByMouseOld = -1;
    CommandSelectedByMouse = -1;

}

//-------------------------------------------------------------------------------------------------
Logic::~Logic(void)
{

}

//-------------------------------------------------------------------------------------------------
void Logic::RunCodeEditor(void)
{
    if (interface->ThisButtonWasPressed == 8)
    {
        if (CommandDisplayStartIndex > 0)
        {
            CommandDisplayStartIndex--;
            CommandDisplayEndIndex--;
        }
    }
    else if (interface->ThisButtonWasPressed == 9)
    {
        if (CommandDisplayStartIndex < 98)
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

}
