#include "SDL2/SDL.h"
#include <cstdlib>
#include <iostream>
#include "NGLDraw.h"
#include "Menu.h"
#include "Game.h"
#include <ngl/NGLInit.h>

void PlayGame(SDL_Window *window, bool quit);

int main( int argc, char* args[] )
{
  // Initialize SDL's Video subsystem
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0 )
  {
    // Or die on error
    SDLErrorExit("Unable to initialize SDL");
  }

  TTF_Init();

  // now get the size of the display and create a window we need to init the video
  SDL_Rect rect;
  SDL_GetDisplayBounds(0,&rect);

  // now create our window
  SDL_Window *window=SDL_CreateWindow("Polarizer",
                                      SDL_WINDOWPOS_CENTERED,
                                      SDL_WINDOWPOS_CENTERED,
                                      rect.w/2,
                                      rect.h/2,
                                      SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
                                     );
  // check to see if that worked or exit
  if (!window)
  {
    SDLErrorExit("Unable to create window");
  }

  // create quit flag
  bool quit = false;

  //create event handler
  SDL_Event event;

  // create the UI renderer
  Menu UI(window, rect);

  // ~~~Create Main Menu buttons~~~
  // ***CONCERN*** should these be under Menu::MainMenu()? or would
  //               calling that function and initializing the button
  //               class every time the main loop loops be bad for performance?
  Button playButton( "PLAY", UI.m_Renderer);
  // set the playButton's position (centered based on button dimensions)
  playButton.setPosition(rect.w/4, rect.h/4, true);
  //~~~

  // while the program is running
  while(!quit)
  {
    // ~~~Display Main Menu~~~
    // set background color to grey
    SDL_SetRenderDrawColor(UI.m_Renderer, 112, 112, 112, 225);
    SDL_RenderClear(UI.m_Renderer);
    // render playButton
    UI.MainMenu(playButton);
    // update screen
    SDL_RenderPresent(UI.m_Renderer);
    // ~~~~~~~~~~~~~~~~~~~~~~~

    // while there is an event
    while(SDL_PollEvent(&event))
    {
      switch( event.type )
      {
        //if the window is closed
        case SDL_QUIT : quit = true; break;

        //if a mouse button was pressed
        case SDL_MOUSEBUTTONUP:
        {
          // if the button was the left mouse button
          if( event.button.button == SDL_BUTTON_LEFT)
          {
            //get mouse position offsets
            int mouseX = event.button.x;
            int mouseY = event.button.y;

            //if mouse pressed playButton
            if(playButton.isInside(mouseX,mouseY))
            {
              //clear screen
              SDL_RenderClear(UI.m_Renderer);

              //open game screen and start game loop
              UI.Game(rect, quit);
            }
          }
        }
      }
    }
  }

  // now tidy up and exit SDL
  SDL_Quit();

}
