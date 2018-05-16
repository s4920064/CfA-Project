#include "Menu.h"
#include "GameLoop.h"
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
Button::Button()
{
  m_textSurface = SDL_CreateRGBSurface(0, 5, 5, 32, 0,0,0,0);
}

Button::Button(const char *_label, int _size)
{
  // set textFont
  m_textFont = TTF_OpenFont("font/game_over.ttf", _size);
  // set buttonSpacing
  m_buttonSpacing = 0;

  // set parameters for textRect
  m_textRect.x = m_buttonSpacing;
  m_textRect.y = m_buttonSpacing;
  TTF_SizeText(m_textFont,"hello",&m_textRect.w,&m_textRect.h);

  // set parameters for borderRect
  m_borderRect.w = m_textRect.w+m_buttonSpacing*2;
  m_borderRect.h = m_textRect.h+m_buttonSpacing*2;
  m_borderRect.x = 0;
  m_borderRect.y = 0;

  // set textColor
  m_textColor = {225,128,0,225};
  // render text on textSurface
  m_textSurface = TTF_RenderText_Solid(m_textFont,_label, m_textColor);

}

Button::Button(const char *_label, SDL_Renderer *_renderer)
{
  //set textFont
  m_textFont = TTF_OpenFont("font/game_over.ttf",70);
  //set buttonSpacing
  m_buttonSpacing = 0;

  //set parameters for textRect
  m_textRect.x = m_buttonSpacing;
  m_textRect.y = m_buttonSpacing;
  TTF_SizeText(m_textFont,"hello",&m_textRect.w,&m_textRect.h);

  //set parameters for borderRect
  m_borderRect.w = m_textRect.w+m_buttonSpacing*2;
  m_borderRect.h = m_textRect.h+m_buttonSpacing*2;
  m_borderRect.x = 0;
  m_borderRect.y = 0;

  //set textColor
  m_textColor = {225,128,0,225};
  //render text on textSurface
  m_textSurface = TTF_RenderText_Solid(m_textFont,_label, m_textColor);

  //convert the textSurface to a texture
  m_buttonTexture = SDL_CreateTextureFromSurface(_renderer, m_textSurface);
  SDL_FreeSurface(m_textSurface);     //free the no longer needed textSurface

}

Button::~Button()
{
  //m_functPtr = NULL;
  // free the surface
  //SDL_FreeSurface(m_textSurface);
}

void Button::setTexture(SDL_Renderer *_renderer)
{
  //convert the textSurface to a texture
  m_buttonTexture = SDL_CreateTextureFromSurface(_renderer, m_textSurface);
}

void Button::setPosition(int _x, int _y, bool _centered)
{
  if( _centered )
  {
    m_borderRect.x = _x-(m_borderRect.w/2);
    m_borderRect.y = _y-(m_borderRect.h/2);
  }
  else
  {
    m_borderRect.x = _x;
    m_borderRect.y = _y;
  }
}

bool Button::isInside(int _mouseX, int _mouseY)
{
  //If the mouse is over the button. End reference.
  if(     ( _mouseX > m_borderRect.x )
       && ( _mouseX < m_borderRect.x + m_borderRect.w )
       && ( _mouseY > m_borderRect.y )
       && ( _mouseY < m_borderRect.y + m_borderRect.h )
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
  const SDL_Rect playRect = m_playButton.m_borderRect;
  // fill button shape
  SDL_SetRenderDrawColor(_renderer,
                         m_buttonColor.r,
                         m_buttonColor.g,
                         m_buttonColor.b,
                         m_buttonColor.a);
  SDL_RenderFillRect(_renderer, &playRect);
  // render the playButton text over top of the button color
  SDL_RenderCopy(_renderer,
                 m_playButton.getTexture(),
                 NULL,
                 &m_playButton.m_borderRect);
  // ------------------------------------------

  // render instructionsButton-----------------
  //const SDL_Rect instrRect = m_instructButton.borderRect;

  // ------------------------------------------

  // render highscoresButton-------------------
  // ------------------------------------------

  // update screen
  SDL_RenderPresent(_renderer);
}

