#include "Menu.h"
#include "GameLoop.h"
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
Button::Button()
{}

Button::Button(const char *_label, SDL_Renderer *_renderer)
{
  //set textFont
  textFont = TTF_OpenFont("font/game_over.ttf",70);
  //set buttonSpacing
  buttonSpacing = 0;

  //set parameters for textRect
  textRect.x = buttonSpacing;
  textRect.y = buttonSpacing;
  TTF_SizeText(textFont,"hello",&textRect.w,&textRect.h);

  //set parameters for borderRect
  borderRect.w = textRect.w+buttonSpacing*2;
  borderRect.h = textRect.h+buttonSpacing*2;
  borderRect.x = 0;
  borderRect.y = 0;

  //set textColor
  textColor = {225,128,0,225};
  //render text on textSurface
  textSurface = TTF_RenderText_Solid(textFont,_label, textColor);

  //convert the textSurface to a texture
  buttonTexture = SDL_CreateTextureFromSurface(_renderer, textSurface);
  SDL_FreeSurface(textSurface);     //free the no longer needed textSurface

}

Button::~Button()
{
  //m_functPtr = NULL;
}

void Button::setPosition(int _x, int _y, bool _centered)
{
  if( _centered )
  {
    borderRect.x = _x-(borderRect.w/2);
    borderRect.y = _y-(borderRect.h/2);
  }
  else
  {
    borderRect.x = _x;
    borderRect.y = _y;
  }
}

bool Button::isInside(int _mouseX, int _mouseY)
{
  //If the mouse is over the button. End reference.
  if(     ( _mouseX > borderRect.x )
       && ( _mouseX < borderRect.x + borderRect.w )
       && ( _mouseY > borderRect.y )
       && ( _mouseY < borderRect.y + borderRect.h )
     )
  {
    return true;
  }
  return false;
}




MainMenu::MainMenu(SDL_Renderer *_renderer, SDL_Rect _windowRect)
{
  m_playButton = Button("PLAY", _renderer);
  m_playButton.setPosition(_windowRect.w/2, _windowRect.h/2, true);
}

MainMenu::~MainMenu()
{
  std::cout<<"Exiting Program\n";
}

void MainMenu::render(SDL_Renderer *_renderer)
{
  // set background color
  SDL_SetRenderDrawColor(_renderer,
                         m_backColor.r,
                         m_backColor.g,
                         m_backColor.b,
                         m_backColor.a);
  SDL_RenderClear(_renderer);

  // render playButton------------------------
  // turn playButton.borderRect into a const SDL_Rect for the SDL_RenderFillRect() function
  const SDL_Rect playRect = m_playButton.borderRect;
  // fill button shape
  SDL_SetRenderDrawColor(_renderer,
                         m_buttonColor.r,
                         m_buttonColor.g,
                         m_buttonColor.b,
                         m_buttonColor.a);
  SDL_RenderFillRect(_renderer, &playRect);
  // render the playButton text over top of the button color
  SDL_RenderCopy(_renderer,
                 m_playButton.buttonTexture,
                 NULL,
                 &m_playButton.borderRect);
  // ------------------------------------------

  // render instructionsButton-----------------
  //const SDL_Rect instrRect = m_instructButton.borderRect;

  // ------------------------------------------

  // render highscoresButton-------------------
  // ------------------------------------------

  // update screen
  SDL_RenderPresent(_renderer);
}

