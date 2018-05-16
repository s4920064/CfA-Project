#include "SDL2/SDL.h"
#include <cstdlib>
#include <iostream>
#include "Game.h"
#include "Menu.h"
#include "GameLoop.h"
#include <ngl/NGLInit.h>

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
  SDL_Rect screenRect;
  SDL_GetDisplayBounds(0,&screenRect);

  // create an SDL_Rect with the dimensions we want for the window
  SDL_Rect windowRect;
  windowRect.w = screenRect.w/4;
  windowRect.h = screenRect.h/2;


  // now create our window
  SDL_Window *window=SDL_CreateWindow("Game",
                                      SDL_WINDOWPOS_CENTERED,
                                      SDL_WINDOWPOS_CENTERED,
                                      windowRect.w,
                                      windowRect.h,
                                      SDL_WINDOW_OPENGL
                                     );
  // check to see if that worked or exit
  if (!window)
  {
    SDLErrorExit("Unable to create window");
  }

  // create our renderer for the UI
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  // create quit flag
  bool quit = false;

  //create event handler
  SDL_Event event;

  // initialize the UI
  //Menu UI(window);

  MainMenu mainMenu(renderer, windowRect);

  // while the program is running
  while(!quit)
  {
    // while there is an event??
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
            if(mainMenu.m_playButton.isInside(mouseX, mouseY))
            {
              //open game screen and start game loop
              PlayGame(window, windowRect, &quit);
            }
          }
        }
      }
    }
    mainMenu.render(renderer);
  }

  // now tidy up and exit SDL
  SDL_Quit();

}
