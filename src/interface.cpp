#include "SDL.h"

#include "interface.h"

#include "visuals.h"
#include "input.h"
#include "audio.h"
#include "logic.h"
#include "screens.h"

extern Visuals* visuals;
extern Input* input;
extern Audio* audio;
extern Logic* logic;
extern Screens* screens;

//            TM
// "Neo's Kiss"

//-------------------------------------------------------------------------------------------------
Interface::Interface(void)
{
    for (int index = 0; index < NumberOfButtons; index++)
    {
        Buttons[index].OneClick = true;
        Buttons[index].SpriteIndex = -1;
        Buttons[index].Text = " ";
        Buttons[index].ScreenIndex = -1;
        Buttons[index].ScreenX = 320;
        Buttons[index].ScreenY = 240;
        Buttons[index].RedHue = 255;
        Buttons[index].GreenHue = 255;
        Buttons[index].BlueHue = 255;
        Buttons[index].Transparency = 255;
        Buttons[index].ScaleX = 1.0;
        Buttons[index].ScaleY = 1.0;
        Buttons[index].AnimationScale = 1.0;
        Buttons[index].AnimationTimer = -1;
    }

    ThisButtonWasPressed = -1;

}

//-------------------------------------------------------------------------------------------------
Interface::~Interface(void)
{

}

//-------------------------------------------------------------------------------------------------
void Interface::CreateButtonWithText(bool oneClick, const char *textToDisplay, Uint16 spriteIndex, int screenX, int screenY, Uint8 red, Uint8 green, Uint8 blue, Uint8 transparency, float scaleX, float scaleY)
{
int freeButton = 0;

    for (int index = 0; index < NumberOfButtons; index++)
    {
        if (Buttons[index].SpriteIndex == -1)
        {
            freeButton = index;
            break;
        }
    }

    Buttons[freeButton].OneClick = oneClick;
    Buttons[freeButton].SpriteIndex = spriteIndex;
    Buttons[freeButton].Text = textToDisplay;
    Buttons[freeButton].ScreenIndex = freeButton;
    Buttons[freeButton].ScreenX = screenX;
    Buttons[freeButton].ScreenY = screenY;
    Buttons[freeButton].RedHue = red;
    Buttons[freeButton].GreenHue = green;
    Buttons[freeButton].BlueHue = blue;
    Buttons[freeButton].Transparency = transparency;
    Buttons[freeButton].ScaleX = scaleX;
    Buttons[freeButton].ScaleY = scaleY;
    Buttons[freeButton].AnimationScale = 1.0;
    Buttons[freeButton].AnimationTimer = -1;
}

//-------------------------------------------------------------------------------------------------
void Interface::DisplayAllButtonsOnScreenBuffer(void)
{
float animScale;

    for (int index = 0; index < NumberOfButtons; index++)
    {
        if (Buttons[index].SpriteIndex > -1)
        {
            animScale = Buttons[index].AnimationScale;

            visuals->Sprites[ Buttons[index].SpriteIndex ].ScreenX = Buttons[index].ScreenX;
            visuals->Sprites[ Buttons[index].SpriteIndex ].ScreenY = Buttons[index].ScreenY;
            visuals->Sprites[ Buttons[index].SpriteIndex ].ScaleX = (Buttons[index].ScaleX * animScale);
            visuals->Sprites[ Buttons[index].SpriteIndex ].ScaleY = (Buttons[index].ScaleY * animScale);
            visuals->Sprites[ Buttons[index].SpriteIndex ].RedHue = Buttons[index].RedHue;
            visuals->Sprites[ Buttons[index].SpriteIndex ].GreenHue = Buttons[index].GreenHue;
            visuals->Sprites[ Buttons[index].SpriteIndex ].BlueHue = Buttons[index].BlueHue;
            visuals->Sprites[ Buttons[index].SpriteIndex ].Transparency = Buttons[index].Transparency;
            visuals->DrawSpriteOntoScreenBuffer(Buttons[index].SpriteIndex);

            visuals->DrawSentenceOntoScreenBuffer( 1, Buttons[index].Text, Buttons[index].ScreenX, Buttons[index].ScreenY, JustifyCenterOnPoint, 255, 255, 255, 255, (4.4*Buttons[index].ScaleX*animScale), (5.5*Buttons[index].ScaleX*animScale) );
        }
    }
}

//-------------------------------------------------------------------------------------------------
void Interface::ProcessAllButtons(void)
{
    ThisButtonWasPressed = -1;

    for (int index = 0; index < NumberOfButtons; index++)
    {
        if (Buttons[index].SpriteIndex > -1)
        {
            if ( (input->MouseButtonPressed[0] == true && Buttons[index].OneClick == true) || (input->MouseButtonsRaw[0] == true && Buttons[index].OneClick == false) )
            {
                if (   (  input->MouseY > ( Buttons[index].ScreenY - ((visuals->Sprites[ Buttons[index].SpriteIndex ].TextureHeightOriginal*Buttons[index].ScaleY) / 2) )  )
                   && (  input->MouseY < ( Buttons[index].ScreenY + ((visuals->Sprites[ Buttons[index].SpriteIndex ].TextureHeightOriginal*Buttons[index].ScaleY) / 2) )  )
                   && (  input->MouseX > ( Buttons[index].ScreenX - ((visuals->Sprites[ Buttons[index].SpriteIndex ].TextureWidthOriginal*Buttons[index].ScaleX) / 2) )  )
                   && (  input->MouseX < ( Buttons[index].ScreenX + ((visuals->Sprites[ Buttons[index].SpriteIndex ].TextureWidthOriginal*Buttons[index].ScaleX) / 2) )  )   )
                {
                    Buttons[index].AnimationScale = 0.85;
                    Buttons[index].AnimationTimer = 10;

                    screens->ScreenIsDirty = true;

                    if (Buttons[index].OneClick == true)  audio->PlayAudio(0);
                    else
                    {
                        if (input->DelayAllUserInput == 0)
                        {
                            ThisButtonWasPressed = Buttons[index].ScreenIndex;
                            Buttons[index].AnimationTimer+=1;

                            if (logic->CommandScrollNumberMoved > 5 && logic->CommandScrollNumberMoved < 10)
                            {
                                logic->CommandScrollSpeed = 4;
                            }
                            else if (logic->CommandScrollNumberMoved > 9 && logic->CommandScrollNumberMoved < 13)
                            {
                                logic->CommandScrollSpeed = 3;
                            }
                            else if (logic->CommandScrollNumberMoved > 12 && logic->CommandScrollNumberMoved < 15)
                            {
                                logic->CommandScrollSpeed = 2;
                            }
                            else if (logic->CommandScrollNumberMoved > 14 && logic->CommandScrollNumberMoved < 16)
                            {
                                logic->CommandScrollSpeed = 1;
                            }
                            else if (logic->CommandScrollNumberMoved > 15)
                            {
                                logic->CommandScrollSpeed = 0;
                            }

                            input->DelayAllUserInput = logic->CommandScrollSpeed;
                            logic->CommandScrollNumberMoved++;
                       }
                        else
                        {
                            ThisButtonWasPressed = -1;
                        }
                    }
                }
            }
        }
    }

    for (int index = 0; index < NumberOfButtons; index++)
    {
        if (Buttons[index].SpriteIndex > -1)
        {
            if (Buttons[index].AnimationTimer > -1)
            {
                Buttons[index].AnimationTimer--;

                if (Buttons[index].AnimationTimer == 1)
                {
                    Buttons[index].AnimationScale = 1.0;
                    if (Buttons[index].OneClick == true)  ThisButtonWasPressed = Buttons[index].ScreenIndex;
                    screens->ScreenIsDirty = true;
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------
void Interface::DestroyAllButtons(void)
{
    for (int index = 0; index < NumberOfButtons; index++)
    {
        Buttons[index].OneClick = true;
        Buttons[index].SpriteIndex = -1;
        Buttons[index].Text = " ";
        Buttons[index].ScreenIndex = -1;
        Buttons[index].ScreenX = 320;
        Buttons[index].ScreenY = 240;
        Buttons[index].RedHue = 255;
        Buttons[index].GreenHue = 255;
        Buttons[index].BlueHue = 255;
        Buttons[index].Transparency = 255;
        Buttons[index].ScaleX = 1.0;
        Buttons[index].ScaleY = 1.0;
        Buttons[index].AnimationScale = 1.0;
        Buttons[index].AnimationTimer = -1;
    }

    ThisButtonWasPressed = -1;

}
