#include "Menu.h"
#include "Game.h"
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

Button::Button(const char *label, SDL_Renderer *renderer)
{
    //set textFont
    textFont = TTF_OpenFont("game_over.ttf",70);
    //set buttonSpacing
    buttonSpacing = 0;

    //set parameters for textRect
    textRect.x = buttonSpacing;
    textRect.y = buttonSpacing;
    TTF_SizeText(textFont,"hello",&textRect.w,&textRect.h);

    //set parameters for borderRect
    borderRect.w =textRect.w+buttonSpacing*2;
    borderRect.h = textRect.h+buttonSpacing*2;
    borderRect.x = 0;
    borderRect.y = 0;

    //create buttonSurface to fill rect
 // buttonSurface = SDL_CreateRGBSurface(0, borderRect.w, borderRect.h, 32,0,0,0,0);

 // buttonSurface = SDL_ConvertSurfaceFormat(aSurface, SDL_PIXELFORMAT_RGBA8888, 0);
 // SDL_FreeSurface(aSurface);

 // const SDL_PixelFormat *fmt = buttonSurface->format;

    //set buttonColor
 // buttonColor = SDL_MapRGB(SDL_PIXELFORMAT_RGB888, 50, 50, 50); //dark grey
    //fill rect on buttonSurface
 // SDL_FillRect(buttonSurface, NULL, buttonColor);

    //set textColor
    textColor = {225,128,0,225};
    //render text on textSurface
    textSurface = TTF_RenderText_Solid(textFont,label, textColor);

    //blit textSurface to buttonSurface
 // SDL_BlitSurface(textSurface,NULL,buttonSurface,&textRect);
 // SDL_FreeSurface(textSurface);       //free the no longer needed textSurface


    //convert the buttonSurface to a texture
    buttonTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);     //free the no longer needed buttonSurface

}

Button::~Button()
{
    m_functPtr = NULL;
}

bool Button::setPosition(int _x, int _y, bool centered)
{
    if( centered )
    {
        borderRect.x = _x-(borderRect.w/2);
        borderRect.y = _y-(borderRect.h/2);
        return true;
    }
    else
    {
        borderRect.x = _x;
        borderRect.y = _y;
        return true;
    }
    return false;
}

bool Button::isInside(int mouseX, int mouseY)
{
    //If the mouse is over the button. End reference.
    if(     ( mouseX > borderRect.x )
         && ( mouseX < borderRect.x + borderRect.w )
         && ( mouseY > borderRect.y )
         && ( mouseY < borderRect.y + borderRect.h )
       )
    {
        return true;
    }
    return false;
}





Menu::Menu(SDL_Window* window, SDL_Rect rect)
{
    m_Renderer = SDL_CreateRenderer(window, -1, 0);
    m_winWidth = rect.w/2;
    m_winHeight = rect.h/2;

}

Menu::~Menu()
{
    SDL_DestroyTexture(UITexture);
}

void Menu::MainMenu(Button playButton)
{
    //render button
    //SDL_RenderFillRect(m_Renderer, playButton.borderRect):
    SDL_SetRenderDrawColor(m_Renderer,120,50,50,255);
    const SDL_Rect Rect = playButton.borderRect;
    SDL_RenderFillRect(m_Renderer, &Rect);
    SDL_RenderCopy(m_Renderer,playButton.buttonTexture,NULL,&playButton.borderRect);
}

void Menu::Instructions(Button backButton)
{
    //clear screen
    SDL_RenderClear(m_Renderer);
    //set the backButton's position
    backButton.setPosition(30, m_winHeight-60, false);
}

void Menu::HighScores()
{

}

void Menu::Game(SDL_Rect rect, bool quit)
{
    //clear screen
    SDL_RenderClear(m_Renderer);

    //start game loop
    PlayGame(m_window, rect, quit);
}


