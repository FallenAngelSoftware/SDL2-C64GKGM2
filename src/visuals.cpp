#include "SDL.h"

#include <stdio.h>
#include <cstring>

#include "visuals.h"

//-------------------------------------------------------------------------------------------------
Visuals::Visuals(void)
{
    CoreFailure = false;

    FrameLock = 16;
    SystemTicks = SDL_GetTicks();
    NextFrameTicks = SystemTicks + FrameLock;
    NumberOfFrames = 0;

    CurrentFramePerSecond = 0;

    for (int index = 0; index < 10; index++)
    {
        FramesPerSecondArray[index] = 0;
    }

    NextSecondTick = 0;
    AverageFPS = 0;

    for (int index = 0; index < NumberOfSprites; index++)
    {
        Sprites[index].ScreenX = 320;
        Sprites[index].ScreenY = 240;
        Sprites[index].ScaleX = 1.0;
        Sprites[index].ScaleY = 1.0;
        Sprites[index].RotationDegree = 0;
        Sprites[index].RedHue = 255;
        Sprites[index].GreenHue = 255;
        Sprites[index].BlueHue = 255;
        Sprites[index].Transparency = 255;
        Sprites[index].Smooth = false;
        Sprites[index].FlipX = false;
        Sprites[index].FlipY = false;
        Sprites[index].AnimationTimer = 0;
    }

    for (int index = 0; index < NumberOfFontChars; index++)
    {
        FontChars[index].ScreenX = 320;
        FontChars[index].ScreenY = 240;
        FontChars[index].ScaleX = 1.0;
        FontChars[index].ScaleY = 1.0;
        FontChars[index].RotationDegree = 0;
        FontChars[index].RedHue = 255;
        FontChars[index].GreenHue = 255;
        FontChars[index].BlueHue = 255;
        FontChars[index].Transparency = 255;
        FontChars[index].Smooth = false;
        FontChars[index].FlipX = false;
        FontChars[index].FlipY = false;
        FontChars[index].AnimationTimer = 0;
    }
}

//-------------------------------------------------------------------------------------------------
Visuals::~Visuals(void)
{
    for (int index = 0; index < NumberOfSprites; index++)
    {
        if (Sprites[index].Texture != NULL)
        {
            SDL_DestroyTexture(Sprites[index].Texture);
        }
    }

    for (int index = 0; index < NumberOfFontChars; index++)
    {
        if (FontChars[index].Texture[0] != NULL)
        {
            SDL_DestroyTexture(FontChars[index].Texture[0]);
        }

        if (FontChars[index].Texture[1] != NULL)
        {
            SDL_DestroyTexture(FontChars[index].Texture[1]);
        }
    }

    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(Window);
}

//-------------------------------------------------------------------------------------------------
bool Visuals::LoadSpritesAndInitialize(void)
{
char filePath[256];
SDL_Surface* tempSurface;
Uint32 textureFormat;
int textureAccess;
int textureWidth;
int textureHeight;

    for (int index = 0; index < NumberOfSprites; index++)
    {
        Sprites[index].Texture = NULL;
    }

    for (int index = 0; index < NumberOfSprites; index++)
    {
        strcpy(filePath, "~");

        switch(index)
        {
            case 1:
                strcpy(filePath, "data/visuals/Screen-Fade.bmp");
                break;

            case 2:
                strcpy(filePath, "data/visuals/Debug-BG.bmp");
                break;

            case 10:
                strcpy(filePath, "data/visuals/FAS-Statue.bmp");
                break;

            case 20:
                strcpy(filePath, "data/visuals/SDL-Logo.bmp");
                break;

            case 100:
                strcpy(filePath, "data/visuals/Editor-BG.bmp");
                break;

            case 1200:
                strcpy(filePath, "data/visuals/Button.bmp");
                break;

            case 1210:
                strcpy(filePath, "data/visuals/Up-Icon.bmp");
                break;

            case 1211:
                strcpy(filePath, "data/visuals/Up-Icon-Inverted.bmp");
                break;

            case 1215:
                strcpy(filePath, "data/visuals/Down-Icon.bmp");
                break;

            case 1216:
                strcpy(filePath, "data/visuals/Down-Icon-Inverted.bmp");
                break;

            case 1220:
                strcpy(filePath, "data/visuals/Code-Select-Box.bmp");
                break;

            case 1221:
                strcpy(filePath, "data/visuals/Line-Number-Button.bmp");
                break;

            case 1250:
                strcpy(filePath, "data/visuals/Command-List-Box.bmp");
                break;

            case 1251:
                strcpy(filePath, "data/visuals/Code-Box.bmp");
                break;

            default:
                break;
        }

        if (filePath[0] != '~')
        {
            tempSurface = SDL_LoadBMP(filePath);
            if (tempSurface == NULL)
            {
                printf( "SDL2 load BMP failed: %s\n", SDL_GetError() );
                CoreFailure = true;
                return(false);
            }

            Sprites[index].Texture = SDL_CreateTextureFromSurface(Renderer, tempSurface);

            SDL_QueryTexture(Sprites[index].Texture, &textureFormat, &textureAccess, &textureWidth, &textureHeight);
            Sprites[index].TextureWidthOriginal = textureWidth;
            Sprites[index].TextureHeightOriginal = textureHeight;

            SDL_FreeSurface(tempSurface);
        }
    }

    return(true);
}

//-------------------------------------------------------------------------------------------------
bool Visuals::LoadFontBMPs(void)
{
Uint32 textureFormat;
int textureAccess;
int textureWidth;
int textureHeight;

    FontBMPsCharWidth = 9;
    FontBMPsCharHeight = 9;

    SDL_Surface *spriteSheet = NULL;
    SDL_Surface *boxTemp = NULL;
    SDL_Rect dstRect;
    SDL_Rect srcRect;

    spriteSheet = SDL_LoadBMP("data/font/FontBMP.bmp");
    if (!spriteSheet)
    {
        printf( "SDL2 load BMP failed: %s\n", SDL_GetError() );
        CoreFailure = true;
        return(false);
    }

    dstRect.x = 0;
    dstRect.y = 0;
    dstRect.w = FontBMPsCharWidth;
    dstRect.h = FontBMPsCharHeight;

    srcRect.x = 1;
    srcRect.y = 1;
    srcRect.w = FontBMPsCharWidth;
    srcRect.h = FontBMPsCharHeight;

    for (int index = 32; index < 127; index++)
    {
        srcRect.y = 1;

        boxTemp = SDL_LoadBMP("data/font/CharTemp.bmp");
        if (!boxTemp)
        {
            printf( "SDL2 load BMP failed: %s\n", SDL_GetError() );
            CoreFailure = true;
            return(false);
        }

        SDL_SetColorKey( boxTemp, true, SDL_MapRGB(boxTemp->format, 0, 0, 0) );

        SDL_BlitSurface(spriteSheet, &srcRect, boxTemp, &dstRect);

//        srcRect.x+=(FontBMPsCharWidth+1);

        FontChars[index].Texture[0] = SDL_CreateTextureFromSurface(Renderer, boxTemp);

        SDL_QueryTexture(FontChars[index].Texture[0], &textureFormat, &textureAccess, &textureWidth, &textureHeight);
        FontChars[index].TextureWidthOriginal = textureWidth;
        FontChars[index].TextureHeightOriginal = textureHeight;

        SDL_FreeSurface(boxTemp);


        srcRect.y = 11;

        boxTemp = SDL_LoadBMP("data/font/CharTemp.bmp");
        if (!boxTemp)
        {
            printf( "SDL2 load BMP failed: %s\n", SDL_GetError() );
            CoreFailure = true;
            return(false);
        }

        SDL_SetColorKey( boxTemp, true, SDL_MapRGB(boxTemp->format, 0, 0, 0) );

        SDL_BlitSurface(spriteSheet, &srcRect, boxTemp, &dstRect);

        srcRect.x+=(FontBMPsCharWidth+1);

        FontChars[index].Texture[1] = SDL_CreateTextureFromSurface(Renderer, boxTemp);

        SDL_QueryTexture(FontChars[index].Texture[1], &textureFormat, &textureAccess, &textureWidth, &textureHeight);
        FontChars[index].TextureWidthOriginal = textureWidth;
        FontChars[index].TextureHeightOriginal = textureHeight;

        SDL_FreeSurface(boxTemp);
    }

    SDL_FreeSurface(spriteSheet);

    return(true);
}

//-------------------------------------------------------------------------------------------------
void Visuals::DrawCharOntoScreenBuffer(int tileSet, Uint16 index)
{
SDL_Rect destinationRect;
Uint32 textureFormat;
int textureAccess;
int textureWidth;
int textureHeight;

    SDL_QueryTexture(FontChars[index].Texture[tileSet], &textureFormat, &textureAccess, &textureWidth, &textureHeight);

    float winWidthFixed = (float)WindowWidthCurrent / 640;
    float winHeightFixed = (float)WindowHeightCurrent / 480;

    destinationRect.x = ( FontChars[index].ScreenX * (winWidthFixed) )
                        - (  ( (textureWidth * FontChars[index].ScaleX) * (winWidthFixed) ) / 2  );
    destinationRect.y = ( FontChars[index].ScreenY * (winHeightFixed) )
                        - (  ( (textureHeight * FontChars[index].ScaleY) * (winHeightFixed) ) / 2  );
    destinationRect.w = textureWidth * FontChars[index].ScaleX * (winWidthFixed);
    destinationRect.h = textureHeight * FontChars[index].ScaleY * (winHeightFixed);

    SDL_SetTextureColorMod(FontChars[index].Texture[tileSet], FontChars[index].RedHue, FontChars[index].GreenHue, FontChars[index].BlueHue);
    SDL_SetTextureAlphaMod(FontChars[index].Texture[tileSet], FontChars[index].Transparency);

    if (FontChars[index].FlipX == false && FontChars[index].FlipY == false)
    {
        SDL_RenderCopyEx(Renderer, FontChars[index].Texture[tileSet], NULL, &destinationRect, FontChars[index].RotationDegree
                         , NULL, SDL_FLIP_NONE);
    }
    else if (FontChars[index].FlipX == true && FontChars[index].FlipY == false)
    {
        SDL_RenderCopyEx(Renderer, FontChars[index].Texture[tileSet], NULL, &destinationRect, FontChars[index].RotationDegree
                         , NULL, SDL_FLIP_HORIZONTAL);
    }
    else if (FontChars[index].FlipX == false && FontChars[index].FlipY == true)
    {
        SDL_RenderCopyEx(Renderer, FontChars[index].Texture[tileSet], NULL, &destinationRect, FontChars[index].RotationDegree
                         , NULL, SDL_FLIP_VERTICAL);
    }
    else if (FontChars[index].FlipX == true && FontChars[index].FlipY == true)
    {
        double flipHorizontallyAndVerticallyDegreeFix = FontChars[index].RotationDegree+180;

        SDL_RenderCopyEx(Renderer, FontChars[index].Texture[tileSet], NULL, &destinationRect, flipHorizontallyAndVerticallyDegreeFix
                         , NULL, SDL_FLIP_NONE);
    }
}

//-------------------------------------------------------------------------------------------------
void Visuals::DrawSentenceOntoScreenBuffer(int tileSet, const char *textToDisplay, int screenX, int screenY, int justificationHorizontal, Uint8 red, Uint8 green, Uint8 blue, Uint8 transparancy, float scaleX, float scaleY)
{
int charScreenX = screenX;

    if (justificationHorizontal == JustifyLeft)
    {
        charScreenX = screenX;
    }
    else if (justificationHorizontal == JustifyCenter)
    {
        charScreenX = 320 - (  ( ( strlen(textToDisplay) - 1) * FontBMPsCharWidth * scaleX  ) / 2  );
    }
    else if (justificationHorizontal == JustifyRight)
    {
        charScreenX = screenX - (  ( ( strlen(textToDisplay) - 1) * FontBMPsCharWidth * scaleX  )  );
    }
    else if (justificationHorizontal == JustifyCenterOnPoint)
    {
        charScreenX = screenX - (  ( ( strlen(textToDisplay) - 1) * FontBMPsCharWidth * scaleX  ) / 2  );
    }

    for (Uint8 index = 0; index < strlen(textToDisplay); index++)
    {
        if ( int(textToDisplay[index]) > 31 && int(textToDisplay[index]) < 127 )
        {
            FontChars[int(textToDisplay[index])].ScreenX = charScreenX;
            FontChars[int(textToDisplay[index])].ScreenY = screenY;
            FontChars[int(textToDisplay[index])].RedHue = red;
            FontChars[int(textToDisplay[index])].GreenHue = green;
            FontChars[int(textToDisplay[index])].BlueHue = blue;
            FontChars[int(textToDisplay[index])].Transparency = transparancy;
            FontChars[int(textToDisplay[index])].ScaleX = scaleX;
            FontChars[int(textToDisplay[index])].ScaleY = scaleY;
            DrawCharOntoScreenBuffer( tileSet, int(textToDisplay[index]) );

            charScreenX+=(FontBMPsCharWidth*scaleX);
        }
    }
}

//-------------------------------------------------------------------------------------------------
void Visuals::CacheAllCommandTexts(void)
{
    Commands[  0].CommandTexts = "ADD 0000 TO SCORE1";
    Commands[  1].CommandTexts = "ADD 0000 TO SCORE[A]";
    Commands[  2].CommandTexts = "ADD [A] TO SCORE1";
    Commands[  3].CommandTexts = "ADD [A] TO SCORE[A]";
    Commands[  4].CommandTexts = "CLEAR SCENE 1";
    Commands[  5].CommandTexts = "CLEAR SCORE1";
    Commands[  6].CommandTexts = "CLEAR SCORE[A]";
    Commands[  7].CommandTexts = "CLEAR SPRITE";
    Commands[  8].CommandTexts = "/ COMMENT";
    Commands[  9].CommandTexts = "DATA TABLE AT 1001";
    Commands[ 10].CommandTexts = "DATA VALUES - 000 000";
    Commands[ 11].CommandTexts = "DISPLAY OTHER SCENE";
    Commands[ 12].CommandTexts = "DISPLAY SCENE 1";
    Commands[ 13].CommandTexts = "END IF";
    Commands[ 14].CommandTexts = "IF A = 000 THEN";
    Commands[ 15].CommandTexts = "IF A = [A] THEN";
    Commands[ 16].CommandTexts = "IF A > 000 THEN";
    Commands[ 17].CommandTexts = "IF A > [A] THEN";
    Commands[ 18].CommandTexts = "IF A < 000 THEN";
    Commands[ 19].CommandTexts = "IF A < [A] THEN";
    Commands[ 20].CommandTexts = "IF BUTTON 1 IS ON  THEN";
    Commands[ 21].CommandTexts = "IF JOYSTICK 1 IS RIGHT THEN";
    Commands[ 22].CommandTexts = "IF SCORE > 000000 THEN";
    Commands[ 23].CommandTexts = "IF SCORE[A] > 000000 THEN";
    Commands[ 24].CommandTexts = "IF SCORE1 > SCORE2 THEN";
    Commands[ 25].CommandTexts = "IF SPRITE HIT SPRITE THEN";
    Commands[ 26].CommandTexts = "JUMP TO LABEL L001";
    Commands[ 27].CommandTexts = "JUMP TO LABEL L[A]";
    Commands[ 28].CommandTexts = "JUMP TO SUBROUTINE AT L001";
    Commands[ 29].CommandTexts = "JUMP TO SUBROUTINE AT L[A]";
    Commands[ 30].CommandTexts = "OTHERWISE";
    Commands[ 31].CommandTexts = "PAUSE FOR 00.0 SECONDS";
    Commands[ 32].CommandTexts = "PLOT A DOT AT X=000 Y=000";
    Commands[ 33].CommandTexts = "PLOT A DOT AT X=[A] Y=[A]";
    Commands[ 34].CommandTexts = "PLOT COLOR 0 TO SCENE 1";
    Commands[ 35].CommandTexts = "PLOT COLOR [A] TO SCENE 1";
    Commands[ 36].CommandTexts = "PRINT ________________________";
    Commands[ 37].CommandTexts = "PRINT AT ROW 00 COLUMN 00";
    Commands[ 38].CommandTexts = "PRINT AT ROW[A] COLUMN[A]";
    Commands[ 39].CommandTexts = "PRINT CHARACTER OF [A]";
    Commands[ 40].CommandTexts = "PRINT COLOR= 00 ON  00";
    Commands[ 41].CommandTexts = "PRINT COLOR=[A] ON [A]";
    Commands[ 42].CommandTexts = "PRINT ON SCENE1";
    Commands[ 43].CommandTexts = "PRINT VALUE OF [A]";
    Commands[ 44].CommandTexts = "RETURN FROM SUBROUTINE";
    Commands[ 45].CommandTexts = "SCENE 1 BACKGROUND=BLACK";
    Commands[ 46].CommandTexts = "SCENE 1 BACKGROUND=[A]";
    Commands[ 47].CommandTexts = "SCENE 1 BORDER = BLACK";
    Commands[ 48].CommandTexts = "SCENE 1 BORDER = [A]";
    Commands[ 49].CommandTexts = "SCENE 1 COLOR 1 = BLACK";
    Commands[ 50].CommandTexts = "SCENE 1 COLOR 1 = [A]";
    Commands[ 51].CommandTexts = "SCENE 1 IS [     ]";
    Commands[ 52].CommandTexts = "SCENE 2 COLOR 1 = BLACK";
    Commands[ 53].CommandTexts = "SCENE 2 COLOR 1 = [A]";
    Commands[ 54].CommandTexts = "SCORE1 AT ROW 00 COLUMN 00";
    Commands[ 55].CommandTexts = "SCORE2 AT ROW 00 COLUMN 00";
    Commands[ 56].CommandTexts = "SCORE1 COLOR= 00 ON 00";
    Commands[ 57].CommandTexts = "SCORE2 COLOR= 00 ON 00";
    Commands[ 58].CommandTexts = "SCORE1 DISPLAYS ON SCENE1";
    Commands[ 59].CommandTexts = "SCREEN UPDATE ON";
    Commands[ 60].CommandTexts = "SET A = 000";
    Commands[ 61].CommandTexts = "SET A = [A]";
    Commands[ 62].CommandTexts = "SET A = A + 000";
    Commands[ 63].CommandTexts = "SET A = A + [A]";
    Commands[ 64].CommandTexts = "SET A = A - 000";
    Commands[ 65].CommandTexts = "SET A = A - [A]";
    Commands[ 66].CommandTexts = "SET A = A X 000";
    Commands[ 67].CommandTexts = "SET A = A X [A]";
    Commands[ 68].CommandTexts = "SET A = A / 000";
    Commands[ 69].CommandTexts = "SET A = A / [A]";
    Commands[ 70].CommandTexts = "SET A =RND NUMBER 0 TO 000";
    Commands[ 71].CommandTexts = "SET A =SPRITE  X POSITION";
    Commands[ 72].CommandTexts = "SET A =SPRITE  Y POSITION";
    Commands[ 73].CommandTexts = "SET A =VALUE AT DATA+[A]";
    Commands[ 74].CommandTexts = "SET A =VALUE AT RAM+[A]";
    Commands[ 75].CommandTexts = "SET VALUE AT RAM+[A] =000";
    Commands[ 76].CommandTexts = "SET VALUE AT RAM+[A] =[A]";
    Commands[ 77].CommandTexts = "SKIP NEXT IF A = 000";
    Commands[ 78].CommandTexts = "SKIP NEXT IF A > 000";
    Commands[ 79].CommandTexts = "SKIP NEXT IF A < 000";
    Commands[ 80].CommandTexts = "SONG IS [     ]";
    Commands[ 81].CommandTexts = "SONG VOLUME = 00";
    Commands[ 82].CommandTexts = "SOUND CHANNEL 1 = [     ]";
    Commands[ 83].CommandTexts = "SOUND CHANNEL 1 OFF";
    Commands[ 84].CommandTexts = "SPRITE 1 IS [     ]";
    Commands[ 85].CommandTexts = "SPRITE  ANIMATES ALWAYS";
    Commands[ 86].CommandTexts = "SPRITE  ANIMATION SPD=000";
    Commands[ 87].CommandTexts = "SPRITE  ANIMATION SPD=[A]";
    Commands[ 88].CommandTexts = "SPRITE  COLOR 1 =BLACK";
    Commands[ 89].CommandTexts = "SPRITE  COLOR 1 =[A]";
    Commands[ 90].CommandTexts = "SPRITE  DIR =000  000'";
    Commands[ 91].CommandTexts = "SPRITE  DIR =[A]";
    Commands[ 92].CommandTexts = "SPRITE  MOVEMENT SPEED=000";
    Commands[ 93].CommandTexts = "SPRITE  MOVEMENT SPEED=[A]";
    Commands[ 94].CommandTexts = "SPRITE SHARED COLR2=BLACK";
    Commands[ 95].CommandTexts = "SPRITE SHARED COLR2=[A]";
    Commands[ 96].CommandTexts = "SPRITE  UNDER COLORS 2/3";
    Commands[ 97].CommandTexts = "SPRITE  X POSITION =000";
    Commands[ 98].CommandTexts = "SPRITE  X POSITION =[A]";
    Commands[ 99].CommandTexts = "SPRITE  Y POSITION =000";
    Commands[100].CommandTexts = "SPRITE  Y POSITION =[A]";
    Commands[101].CommandTexts = "STOP PROGRAM";
    Commands[102].CommandTexts = "TRACE OF [A] ON";

}

//-------------------------------------------------------------------------------------------------
bool Visuals::InitializeWindow(void)
{
SDL_Surface* windowIcon = SDL_LoadBMP("data/visuals/icon.bmp");

    Window = NULL;
    Window = SDL_CreateWindow("''C64GKGM2'' - (C)2019 By Team www.FallenAngelSoftware.com"
                                 , SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_RESIZABLE);

    if (Window == NULL)
    {
        printf( "SDL2 create window failed: %s\n", SDL_GetError() );
        CoreFailure = true;
        return(false);
    }

    WindowWidthCurrent = 640;
    WindowHeightCurrent = 480;

    SDL_SetWindowIcon(Window, windowIcon);

    Renderer = NULL;
    Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);

    if (Renderer == NULL)
    {
        printf( "SDL2 create renderer failed: %s\n", SDL_GetError() );
        CoreFailure = true;
        return(false);
    }

    return(true);
}

//-------------------------------------------------------------------------------------------------
void Visuals::CalculateFramerate(void)
{
    SystemTicks = SDL_GetTicks();
    NextFrameTicks = SystemTicks + FrameLock;

    NumberOfFrames++;

    if (SystemTicks > NextSecondTick)
    {
        NextSecondTick = SystemTicks + 1000;

        FramesPerSecondArray[CurrentFramePerSecond] = NumberOfFrames;
        NumberOfFrames = 0;

        if (CurrentFramePerSecond < 9)  CurrentFramePerSecond++;
        else  CurrentFramePerSecond = 0;

        Uint32 frameTotal = 0;
        for (int index = 0; index < 10; index++)
        {
            frameTotal += FramesPerSecondArray[index];
        }

        AverageFPS = frameTotal / 10;
    }
}

//-------------------------------------------------------------------------------------------------
void Visuals::ProcessFramerate(void)
{
    SystemTicks = SDL_GetTicks();
    if (NextFrameTicks > SystemTicks)  SDL_Delay(NextFrameTicks - SystemTicks);
}

//-------------------------------------------------------------------------------------------------
void Visuals::ClearScreenBufferWithColor(Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha)
{
    SDL_SetRenderDrawColor(Renderer, red, green, blue, alpha);
    SDL_RenderFillRect(Renderer, NULL);
}

//-------------------------------------------------------------------------------------------------
void Visuals::DrawSpriteOntoScreenBuffer(Uint16 index)
{
SDL_Rect destinationRect;
Uint32 textureFormat;
int textureAccess;
int textureWidth;
int textureHeight;

    SDL_QueryTexture(Sprites[index].Texture, &textureFormat, &textureAccess, &textureWidth, &textureHeight);

    float winWidthFixed = (float)WindowWidthCurrent / 640;
    float winHeightFixed = (float)WindowHeightCurrent / 480;

    destinationRect.x = ( Sprites[index].ScreenX * (winWidthFixed) )
                        - (  ( (textureWidth * Sprites[index].ScaleX) * (winWidthFixed) ) / 2  );
    destinationRect.y = ( Sprites[index].ScreenY * (winHeightFixed) )
                        - (  ( (textureHeight * Sprites[index].ScaleY) * (winHeightFixed) ) / 2  );
    destinationRect.w = textureWidth * Sprites[index].ScaleX * (winWidthFixed);
    destinationRect.h = textureHeight * Sprites[index].ScaleY * (winHeightFixed);

    SDL_SetTextureColorMod(Sprites[index].Texture, Sprites[index].RedHue, Sprites[index].GreenHue, Sprites[index].BlueHue);
    SDL_SetTextureAlphaMod(Sprites[index].Texture, Sprites[index].Transparency);

    if (Sprites[index].FlipX == false && Sprites[index].FlipY == false)
    {
        SDL_RenderCopyEx(Renderer, Sprites[index].Texture, NULL, &destinationRect, Sprites[index].RotationDegree
                         , NULL, SDL_FLIP_NONE);
    }
    else if (Sprites[index].FlipX == true && Sprites[index].FlipY == false)
    {
        SDL_RenderCopyEx(Renderer, Sprites[index].Texture, NULL, &destinationRect, Sprites[index].RotationDegree
                         , NULL, SDL_FLIP_HORIZONTAL);
    }
    else if (Sprites[index].FlipX == false && Sprites[index].FlipY == true)
    {
        SDL_RenderCopyEx(Renderer, Sprites[index].Texture, NULL, &destinationRect, Sprites[index].RotationDegree
                         , NULL, SDL_FLIP_VERTICAL);
    }
    else if (Sprites[index].FlipX == true && Sprites[index].FlipY == true)
    {
        double flipHorizontallyAndVerticallyDegreeFix = Sprites[index].RotationDegree+180;

        SDL_RenderCopyEx(Renderer, Sprites[index].Texture, NULL, &destinationRect, flipHorizontallyAndVerticallyDegreeFix
                         , NULL, SDL_FLIP_NONE);
    }
}
