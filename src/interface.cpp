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
        Buttons[index].TextScaleX = 1.0;
        Buttons[index].TextScaleY = 1.0;
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
        Buttons[index].BlockPressing = false;
        Buttons[index].InterfaceLevel = 0;
        Buttons[index].PlaySound = true;
    }

    ThisButtonWasPressed = -1;

    CurrentInterfaceLevel = 0;

    EditorResizeButtonOriginalPressY = -1;
    EditorResizeButtonY = -1;
    EditorResizeButtonYoffset = 0;
    CodingWindowsValue = 0;

}

//-------------------------------------------------------------------------------------------------
Interface::~Interface(void)
{

}

//-------------------------------------------------------------------------------------------------
void Interface::SetupCodingWindows(void)
{
    logic->CodeBoxOffsetY = 0 + (22*CodingWindowsValue);

    logic->CommandBoxMaxY = 7 + (CodingWindowsValue);
    logic->CommandDisplayEndIndex = logic->CommandBoxMaxY;

    logic->CodeBoxMaxY = 7 - (CodingWindowsValue);
    logic->CodeDisplayEndIndex = logic->CodeBoxMaxY;

    CommandBoxScaleY = 5 + (CodingWindowsValue * 0.61);

    CodeBoxScaleY = 5 + ( (CodingWindowsValue * -1) * 0.54 );

    logic->ShowHideCodeSelectLineNumberBoxes();
    screens->ScreenIsDirty = true;
}

//-------------------------------------------------------------------------------------------------
void Interface::CreateButtonWithText(int level, bool oneClick, bool playSound, const char *textToDisplay, float textScaleX, float textScaleY, Uint16 spriteIndex, int screenX, int screenY, Uint8 red, Uint8 green, Uint8 blue, Uint8 transparency, float scaleX, float scaleY)
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
    Buttons[freeButton].TextScaleX = textScaleX;
    Buttons[freeButton].TextScaleY = textScaleY;
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
//    Buttons[freeButton].BlockPressing = false;
    Buttons[freeButton].InterfaceLevel = level;
    Buttons[freeButton].PlaySound = playSound;
}

//-------------------------------------------------------------------------------------------------
void Interface::DisplayAllButtonsOnScreenBuffer(int level)
{
float animScale;

    if (screens->ScreenIsDirty ==  false)  return;

    for (int index = 0; index < NumberOfButtons; index++)
    {
        if (Buttons[index].SpriteIndex > -1 && Buttons[index].InterfaceLevel == level)
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

            visuals->DrawSentenceOntoScreenBuffer( 1, Buttons[index].Text, Buttons[index].ScreenX, Buttons[index].ScreenY, JustifyCenterOnPoint, Buttons[index].RedHue, Buttons[index].GreenHue, Buttons[index].BlueHue, 255, (Buttons[index].TextScaleX*Buttons[index].ScaleX*animScale), (Buttons[index].TextScaleY*Buttons[index].ScaleY*animScale) );
        }
    }
}

//-------------------------------------------------------------------------------------------------
void Interface::ProcessAllButtons(void)
{
    Buttons[34].RedHue = 255;
    Buttons[34].BlueHue = 255;

    ThisButtonWasPressed = -1;

    for (int index = 0; index < NumberOfButtons; index++)
    {
        if (Buttons[index].SpriteIndex > -1)
        {
            if (  (Buttons[index].BlockPressing == false && Buttons[index].InterfaceLevel == CurrentInterfaceLevel) && ( (input->MouseButtonPressed[0] == true && Buttons[index].OneClick == true) || (input->MouseButtonsRaw[0] == true && Buttons[index].OneClick == false) )  )
            {
                if (   (  input->MouseY > ( Buttons[index].ScreenY - ((visuals->Sprites[ Buttons[index].SpriteIndex ].TextureHeightOriginal*Buttons[index].ScaleY) / 2) )  )
                   && (  input->MouseY < ( Buttons[index].ScreenY + ((visuals->Sprites[ Buttons[index].SpriteIndex ].TextureHeightOriginal*Buttons[index].ScaleY) / 2) )  )
                   && (  input->MouseX > ( Buttons[index].ScreenX - ((visuals->Sprites[ Buttons[index].SpriteIndex ].TextureWidthOriginal*Buttons[index].ScaleX) / 2) )  )
                   && (  input->MouseX < ( Buttons[index].ScreenX + ((visuals->Sprites[ Buttons[index].SpriteIndex ].TextureWidthOriginal*Buttons[index].ScaleX) / 2) )  )   )
                {
                    Buttons[index].AnimationScale = 0.85;
                    Buttons[index].AnimationTimer = 10;

                    screens->ScreenIsDirty = true;

                    if (CurrentInterfaceLevel == 1)  InterfaceLevelBackgroundShown = false;

                    if (Buttons[index].OneClick == true)
                    {
                        if (Buttons[index].PlaySound == true)  audio->PlayAudio(0);
                    }
                    else
                    {
                        if (index > 7 && index < 12)
                        {
                            if (input->DelayAllUserInput == 0)
                            {
                                ThisButtonWasPressed = Buttons[index].ScreenIndex;
                                Buttons[index].AnimationTimer+=1;

                                if (logic->ScrollNumberMoved > 5 && logic->ScrollNumberMoved < 10)
                                {
                                    logic->ScrollSpeed = 4;
                                }
                                else if (logic->ScrollNumberMoved > 9 && logic->ScrollNumberMoved < 13)
                                {
                                    logic->ScrollSpeed = 3;
                                }
                                else if (logic->ScrollNumberMoved > 12 && logic->ScrollNumberMoved < 15)
                                {
                                    logic->ScrollSpeed = 2;
                                }
                                else if (logic->ScrollNumberMoved > 14 && logic->ScrollNumberMoved < 16)
                                {
                                    logic->ScrollSpeed = 1;
                                }
                                else if (logic->ScrollNumberMoved > 15)
                                {
                                    logic->ScrollSpeed = 0;
                                }

                                input->DelayAllUserInput = logic->ScrollSpeed;
                                logic->ScrollNumberMoved++;
                            }
                            else
                            {
                                ThisButtonWasPressed = -1;
                            }
                        }
                        else
                        {
                            if (input->DelayAllUserInput == 0)
                            {
                                ThisButtonWasPressed = Buttons[index].ScreenIndex;
                                Buttons[index].AnimationTimer+=1;

                                Buttons[index].RedHue = 128;
                                Buttons[index].BlueHue = 128;

                                Buttons[index].AnimationScale = 1.0;

                                if (EditorResizeButtonOriginalPressY == -1)
                                {
                                    EditorResizeButtonOriginalPressY = input->MouseY;
                                    EditorResizeButtonY = (240+36);

                                    if (EditorResizeButtonOriginalPressY < EditorResizeButtonY)
                                    {
                                        EditorResizeButtonYoffset = (EditorResizeButtonOriginalPressY - EditorResizeButtonY);
                                    }
                                    else if (EditorResizeButtonOriginalPressY > EditorResizeButtonY)
                                    {
                                        EditorResizeButtonYoffset = (EditorResizeButtonOriginalPressY - EditorResizeButtonY);
                                    }
                                }
                                else if (input->MouseY < EditorResizeButtonOriginalPressY)
                                {
                                    if (CodingWindowsValue > -4)
                                    {
                                        Buttons[34].ScreenY = input->MouseY;
                                        EditorResizeButtonY = input->MouseY;

                                        if ( input->MouseY < (EditorResizeButtonOriginalPressY-21) )
                                        {
                                            EditorResizeButtonOriginalPressY-=21;
                                            CodingWindowsValue--;

                                            SetupCodingWindows();
                                        }
                                    }
                                }
                                else if (input->MouseY > EditorResizeButtonOriginalPressY)
                                {
                                    if (CodingWindowsValue < 0)
                                    {
                                        Buttons[34].ScreenY = input->MouseY;
                                        EditorResizeButtonY = input->MouseY;

                                        if ( input->MouseY > (EditorResizeButtonOriginalPressY+21) )
                                        {
                                            EditorResizeButtonOriginalPressY+=21;
                                            CodingWindowsValue++;

                                            SetupCodingWindows();
                                        }
                                    }
                                }
                            }
                            else
                            {
                                ThisButtonWasPressed = -1;
                            }
                        }
                    }
                }
            }
            else if (index == 34 && EditorResizeButtonOriginalPressY != -1 && Buttons[34].RedHue == 255 && Buttons[34].BlueHue == 255)
            {
                EditorResizeButtonOriginalPressY = -1;
                EditorResizeButtonY = (240+36)+(21*CodingWindowsValue);
                EditorResizeButtonYoffset = 0;
                Buttons[34].ScreenY = EditorResizeButtonY;
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

                if (Buttons[index].AnimationTimer == 5)
                {
                    Buttons[index].AnimationScale = 1.0;
                    screens->ScreenIsDirty = true;

                    if (CurrentInterfaceLevel == 1)  InterfaceLevelBackgroundShown = false;
                }
                else if (Buttons[index].AnimationTimer == 0)
                {
                    if (Buttons[index].OneClick == true)  ThisButtonWasPressed = Buttons[index].ScreenIndex;
                    screens->ScreenIsDirty = true;

                    if (CurrentInterfaceLevel == 1)  InterfaceLevelBackgroundShown = false;
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
        Buttons[index].TextScaleX = 1.0;
        Buttons[index].TextScaleY = 1.0;
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
        Buttons[index].BlockPressing = false;
        Buttons[index].InterfaceLevel = 0;
        Buttons[index].PlaySound = true;
    }

    ThisButtonWasPressed = -1;
}
