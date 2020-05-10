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
#include <iostream>
using namespace std;

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

    initAudio();

    Audio * sound = createAudio("data/audio/SFX-MenuMove.wav", 0, SDL_MIX_MAXVOLUME / 2);
    playSoundFromMemory(sound, SDL_MIX_MAXVOLUME);

    Audio * music = createAudio("data/audio/BGM-IDE1.wav", 1, SDL_MIX_MAXVOLUME);
    playMusicFromMemory(music, SDL_MIX_MAXVOLUME);

    while (input->EXIT_Game == false && visuals->CoreFailure == false)
    {
        input->GetAllUserInput();
        visuals->CalculateFramerate();
        screens->ProcessScreenToDisplay();
        visuals->ProcessFramerate();
    }

    endAudio();
    freeAudio(sound);
    freeAudio(music);

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
