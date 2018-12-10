#include "SDL.h"

#include "logic.h"

#include "interface.h"

extern Interface* interface;

//-------------------------------------------------------------------------------------------------
Logic::Logic(void)
{
    CommandScrollNumberMoved = 0;
    CommandScrollSpeed = 3;

    CommandDisplayStartIndex = 0;
    CommandDisplayEndIndex = 5;

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

}
