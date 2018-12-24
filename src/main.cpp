/*
                        GT-R NISMO "Retro Blast Tech" SDL2 Engine

      ______   ______   _    _      ______  ___  ____    ______  ____    ____   _____ TM
    .' ___  |.' ____ \ | |  | |   .' ___  ||_  ||_  _| .' ___  ||_   \  /   _| / ___ `.
   / .'   \_|| |____\_|| |__| |_ / .'   \_|  | |_/ /  / .'   \_|  |   \/   |  |_/___) |
   | |       | '____`'.|____   _|| |   ____  |  __'.  | |   ____  | |\  /| |   .'____.'
   \ `.___.'\| (____) |    _| |_ \ `.___]  |_| |  \ \_\ `.___]  |_| |_\/_| |_ / /_____
    `.____ .''.______.'   |_____| `._____.'|____||____|`._____.'|_____||_____||_______|

                       (C)2019 By Team www.FallenAngelSoftware.com
*/

#include "SDL.h"

#include <stdio.h>

#include "visuals.h"
#include "input.h"
#include "screens.h"
#include "audio.h"
#include "interface.h"
#include "logicCode.h"
#include "logicSprite.h"

Visuals *visuals;
Input *input;
Screens *screens;
Audio *audio;
Interface *interface;
LogicCode *logicCode;
LogicSprite *logicSprite;

//-------------------------------------------------------------------------------------------------
int main( int argc, char* args[] )
{
    argc = argc; args = args;

    if ( SDL_Init(SDL_INIT_EVERYTHING) != 0 )
    {
        printf( "Unable to initialize SDL2: %s\n", SDL_GetError() );
        return(1);
    }

//    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

    visuals = new Visuals();
    if ( visuals->InitializeWindow() != true ) visuals->CoreFailure = true;
    if ( visuals->LoadSpritesAndInitialize() != true ) visuals->CoreFailure = true;
    if ( visuals->LoadFontBMPs() != true ) visuals->CoreFailure = true;

    visuals->CacheAllCommandTexts();

    input = new Input();

    interface = new Interface();

    logicCode = new LogicCode();

    logicSprite = new LogicSprite();

    screens = new Screens();

    audio = new Audio();
    if (audio->SetupAudio() != true)  visuals->CoreFailure = true;

    while (input->EXIT_Game == false && visuals->CoreFailure == false)
    {
        input->GetAllUserInput();
        visuals->CalculateFramerate();
        screens->ProcessScreenToDisplay();
        visuals->ProcessFramerate();
    }

    delete audio;
    delete screens;
    delete logicSprite;
    delete logicCode;
    delete interface;
    delete input;
    delete visuals;

    SDL_Quit();
    return(0);
}

//                                            TM
// "A 110% By Team www.FallenAngelSoftware.com"...
