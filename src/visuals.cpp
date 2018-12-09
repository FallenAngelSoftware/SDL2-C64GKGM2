#include <stdio.h>
#include <cstring>

#include "SDL.h"

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
        if (FontChars[index].Texture != NULL)
        {
            SDL_DestroyTexture(FontChars[index].Texture);
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

            case 10:
                strcpy(filePath, "data/visuals/FAS-Statue.bmp");
                break;

            case 20:
                strcpy(filePath, "data/visuals/SDL-Logo.bmp");
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
    dstRect.w = 9;
    dstRect.h = 9;

    srcRect.x = 1;
    srcRect.y = 1;
    srcRect.w = 9;
    srcRect.h = 9;

    for (int index = 32; index < 127; index++)
    {
        boxTemp = SDL_LoadBMP("data/font/CharTemp.bmp");
        if (!boxTemp)
        {
            printf( "SDL2 load BMP failed: %s\n", SDL_GetError() );
            CoreFailure = true;
            return(false);
        }

        SDL_SetColorKey( boxTemp, true, SDL_MapRGB(boxTemp->format, 255, 0, 0) );

        SDL_BlitSurface(spriteSheet, &srcRect, boxTemp, &dstRect);

        srcRect.x+=10;

        FontChars[index].Texture = SDL_CreateTextureFromSurface(Renderer, boxTemp);

        SDL_QueryTexture(FontChars[index].Texture, &textureFormat, &textureAccess, &textureWidth, &textureHeight);
        FontChars[index].TextureWidthOriginal = textureWidth;
        FontChars[index].TextureHeightOriginal = textureHeight;

        SDL_FreeSurface(boxTemp);
    }

    SDL_FreeSurface(spriteSheet);

    return(true);
}

//-------------------------------------------------------------------------------------------------
void Visuals::DrawCharOntoScreenBuffer(Uint16 index)
{
SDL_Rect destinationRect;
Uint32 textureFormat;
int textureAccess;
int textureWidth;
int textureHeight;

    SDL_QueryTexture(FontChars[index].Texture, &textureFormat, &textureAccess, &textureWidth, &textureHeight);

    float winWidthFixed = (float)WindowWidthCurrent / 640;
    float winHeightFixed = (float)WindowHeightCurrent / 480;

    destinationRect.x = ( FontChars[index].ScreenX * (winWidthFixed) )
                        - (  ( (textureWidth * FontChars[index].ScaleX) * (winWidthFixed) ) / 2  );
    destinationRect.y = ( FontChars[index].ScreenY * (winHeightFixed) )
                        - (  ( (textureHeight * FontChars[index].ScaleY) * (winHeightFixed) ) / 2  );
    destinationRect.w = textureWidth * FontChars[index].ScaleX * (winWidthFixed);
    destinationRect.h = textureHeight * FontChars[index].ScaleY * (winHeightFixed);

    SDL_SetTextureColorMod(FontChars[index].Texture, FontChars[index].RedHue, FontChars[index].GreenHue, FontChars[index].BlueHue);
    SDL_SetTextureAlphaMod(FontChars[index].Texture, FontChars[index].Transparency);

    if (FontChars[index].FlipX == false && FontChars[index].FlipY == false)
    {
        SDL_RenderCopyEx(Renderer, FontChars[index].Texture, NULL, &destinationRect, FontChars[index].RotationDegree
                         , NULL, SDL_FLIP_NONE);
    }
    else if (FontChars[index].FlipX == true && FontChars[index].FlipY == false)
    {
        SDL_RenderCopyEx(Renderer, FontChars[index].Texture, NULL, &destinationRect, FontChars[index].RotationDegree
                         , NULL, SDL_FLIP_HORIZONTAL);
    }
    else if (FontChars[index].FlipX == false && FontChars[index].FlipY == true)
    {
        SDL_RenderCopyEx(Renderer, FontChars[index].Texture, NULL, &destinationRect, FontChars[index].RotationDegree
                         , NULL, SDL_FLIP_VERTICAL);
    }
    else if (FontChars[index].FlipX == true && FontChars[index].FlipY == true)
    {
        double flipHorizontallyAndVerticallyDegreeFix = FontChars[index].RotationDegree+180;

        SDL_RenderCopyEx(Renderer, FontChars[index].Texture, NULL, &destinationRect, flipHorizontallyAndVerticallyDegreeFix
                         , NULL, SDL_FLIP_NONE);
    }
}

//-------------------------------------------------------------------------------------------------
void Visuals::DrawSentenceOntoScreenBuffer(const char *textToDisplay, int screenX, int screenY, int justificationHorizontal, Uint8 red, Uint8 green, Uint8 blue, Uint8 transparancy, float scaleX, float scaleY)
{
int charScreenX = screenX;

    if (justificationHorizontal == JustifyLeft)
    {
        charScreenX = screenX;
    }
    else if (justificationHorizontal == JustifyCenter)
    {
        charScreenX = 320 - (  ( ( strlen(textToDisplay) - 1) * 9 * scaleX  ) / 2  );
    }
    else if (justificationHorizontal == JustifyRight)
    {
        charScreenX = screenX - (  ( ( strlen(textToDisplay) - 1) * 9 * scaleX  )  );
    }
    else if (justificationHorizontal == JustifyCenterOnPoint)
    {
        charScreenX = screenX - (  ( ( strlen(textToDisplay) - 1) * 9 * scaleX  ) / 2  );
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
            DrawCharOntoScreenBuffer( int(textToDisplay[index]) );

            charScreenX+=(9*scaleX);
        }
    }
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
