#include "SDL.h"

#include "interface.h"

#include "visuals.h"
#include "input.h"
#include "audio.h"
#include "logicCode.h"
#include "screens.h"

extern Visuals* visuals;
extern Input* input;
extern Audio* audio;
extern LogicCode* logicCode;
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

    CodeSelectorButtonsStart = -1;
    CodeSelectorButtonsEnd = -1;
    CodeLineSelectorButtonsStart = -1;
    CodeLineSelectorButtonsEnd = -1;

    CurrentInterfaceLevel = 0;

    EditorResizeButtonOriginalPressY = -1;
    EditorResizeButtonY = -1;
    EditorResizeButtonYoffset = 0;
    CodingWindowsValue = 0;

    EditStatus = EditStatusOff;

}

//-------------------------------------------------------------------------------------------------
Interface::~Interface(void)
{

}

//-------------------------------------------------------------------------------------------------
void Interface::SetupCodingWindows(void)
{
    logicCode->CodeBoxOffsetY = 0 + (18*CodingWindowsValue);

    logicCode->CommandBoxMaxY = 10 + (CodingWindowsValue);
    logicCode->CommandDisplayEndIndex = logicCode->CommandBoxMaxY;

    logicCode->CodeBoxMaxY = 10 - (CodingWindowsValue);
    logicCode->CodeDisplayEndIndex = logicCode->CodeBoxMaxY;

    logicCode->CommandDisplayEndIndex = (logicCode->CommandDisplayStartIndex+logicCode->CommandBoxMaxY);
    logicCode->CodeDisplayEndIndex = (logicCode->CodeDisplayStartIndex+logicCode->CodeBoxMaxY);

    logicCode->ShowHideCodeSelectLineNumberBoxes();
    screens->ScreenIsDirty = true;
}

//-------------------------------------------------------------------------------------------------
int Interface::CreateButtonWithText(int level, bool oneClick, bool playSound, const char *textToDisplay, float textScaleX, float textScaleY, Uint16 spriteIndex, int screenX, int screenY, Uint8 red, Uint8 green, Uint8 blue, Uint8 transparency, float scaleX, float scaleY)
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

    return(freeButton);
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
    Buttons[CodeLineSelectorButtonsEnd+1].RedHue = 255;
    Buttons[CodeLineSelectorButtonsEnd+1].BlueHue = 255;

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

                    }
                    else
                    {
                        if (index > 7 && index < 12)
                        {
                            if (input->DelayAllUserInput == 0)
                            {
                                ThisButtonWasPressed = Buttons[index].ScreenIndex;
                                Buttons[index].AnimationTimer+=1;

                                if (logicCode->ScrollNumberMoved > 5 && logicCode->ScrollNumberMoved < 10)
                                {
                                    logicCode->ScrollSpeed = 4;
                                }
                                else if (logicCode->ScrollNumberMoved > 9 && logicCode->ScrollNumberMoved < 13)
                                {
                                    logicCode->ScrollSpeed = 3;
                                }
                                else if (logicCode->ScrollNumberMoved > 12 && logicCode->ScrollNumberMoved < 15)
                                {
                                    logicCode->ScrollSpeed = 2;
                                }
                                else if (logicCode->ScrollNumberMoved > 14 && logicCode->ScrollNumberMoved < 16)
                                {
                                    logicCode->ScrollSpeed = 1;
                                }
                                else if (logicCode->ScrollNumberMoved > 15)
                                {
                                    logicCode->ScrollSpeed = 0;
                                }

                                input->DelayAllUserInput = logicCode->ScrollSpeed;
                                logicCode->ScrollNumberMoved++;
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

                                    Buttons[CodeLineSelectorButtonsEnd+1].ScaleY = 2.5;

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
                                    if (CodingWindowsValue > -7)
                                    {
                                        Buttons[CodeLineSelectorButtonsEnd+1].ScreenY = input->MouseY;
                                        EditorResizeButtonY = input->MouseY;

                                        if ( input->MouseY < (EditorResizeButtonOriginalPressY-9) )
                                        {
                                            EditorResizeButtonOriginalPressY-=18;
                                            CodingWindowsValue--;

                                            SetupCodingWindows();
                                        }
                                    }
                                }
                                else if (input->MouseY > EditorResizeButtonOriginalPressY)
                                {
                                    if (CodingWindowsValue < 0)
                                    {
                                        Buttons[CodeLineSelectorButtonsEnd+1].ScreenY = input->MouseY;
                                        EditorResizeButtonY = input->MouseY;

                                        if ( input->MouseY > (EditorResizeButtonOriginalPressY+9) )
                                        {
                                            EditorResizeButtonOriginalPressY+=18;
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
            else if ( index == (CodeLineSelectorButtonsEnd+1) && EditorResizeButtonOriginalPressY != -1 && Buttons[34].RedHue == 255 && Buttons[34].BlueHue == 255 )
            {
                SetupCodingWindows();
                Buttons[CodeLineSelectorButtonsEnd+1].ScaleY = 1.0;
                EditorResizeButtonOriginalPressY = -1;
                EditorResizeButtonY = (240+36+5)+(18*CodingWindowsValue);
                EditorResizeButtonYoffset = 0;
                Buttons[CodeLineSelectorButtonsEnd+1].ScreenY = EditorResizeButtonY;
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
