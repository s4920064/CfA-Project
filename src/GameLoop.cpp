#include "GameLoop.h"
#include "Game.h"
#include "Ship.h"
#include "SDL2/SDL.h"
#include <cstdlib>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <ngl/NGLInit.h>
#include "Menu.h"


const static float UPDATE= 0.1;

//FUNCTION FROM JON MACEY
void SDLErrorExit(const std::string &_msg)
{
  std::cerr<<_msg<<"\n";
  std::cerr<<SDL_GetError()<<"\n";
  SDL_Quit();
  exit(EXIT_FAILURE);
}

void PlayGame(SDL_Window *window, SDL_Rect windowRect, bool *quit)
{
  //CODE FROM JON MACEY (STARTS HERE)
  // Create our openGL context and attach it to our window
  SDL_GLContext glContext=createOpenGLContext(window);
  if(!glContext)
  {
    SDLErrorExit("Problem creating OpenGL context");
  }
  // make this our current GL context (we can have more than one window but in this case not)
  SDL_GL_MakeCurrent(window, glContext);
  /* This makes our buffer swap syncronized with the monitor's vertical refresh */
  SDL_GL_SetSwapInterval(1);
  // we need to initialise the game lib which will load all of the Opegame functions, this must
  // be done once we have a valid GL context but before we call any GL commands. If we dont do
  // this everything will crash
  ngl::NGLInit::instance();
  // now clear the screen and swap whilst game inits (which may take time)
  glClear(GL_COLOR_BUFFER_BIT);
  SDL_GL_SwapWindow(window);

  // flag to indicate if we need to exit
  bool back=false;
  // sdl event processing data structure
  SDL_Event event;
  // now we create an instance of our game class, this will init game and setup basic
  // openGL stuff ext. When this falls out of scope the dtor will be called and cleanup

  SDL_SetWindowSize(window, int(float(windowRect.h)*1.5f), windowRect.h);

  Game game(windowRect.h);

  // resize the game to set the screen size and camera stuff
  game.resize(windowRect.h);

  int gameMoveL = 0;
  int gameMoveR = 0;
  int gameMoveF = 0;
  int gameMoveB = 0;

  float gameMoveX = 0;
  float gameMoveZ = 0;

  while(!back)
  {
    while ( SDL_PollEvent(&event) )
    {
      switch (event.type)
      {
        // this is the window x being clicked.
        case SDL_QUIT : *quit = true; back = true; break;
        /// process the mouse data by passing it to game class
//        case SDL_MOUSEMOTION :
//        break;
        case SDL_MOUSEBUTTONDOWN :
//          mouseDown = true;
        break;
        case SDL_MOUSEBUTTONUP :
//          mouseDown = false;
        break;
        ///case SDL_MOUSEWHEEL : game.wheelEvent(event.wheel);

        /// if the window is re-sized pass it to the game class to change gl viewport
        /// note this is slow as the context is re-create by SDL each time
//        case SDL_WINDOWEVENT :
////          if(mouseDown) { resizing = true; }
////          else
////          {
////            resizing = false;
//            int w,h;
//            //get the new window size
//            SDL_GetWindowSize(window,&w,&h);
//            SDL_SetWindowSize(window, int(float(h)*1.3f), h);
//            game.resize(h);
//            std::cout<<w<<"  "<<h<<"\n";
////          }
//        break;

        // now we look for a keydown event
        case SDL_KEYDOWN:
        {
          switch( event.key.keysym.sym )
          {
            // if it's the escape key, exit to menu
            case SDLK_ESCAPE :  back = true; break;
            ///case SDLK_w : glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); break;
            ///case SDLK_s : glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); break;
            ///case SDLK_f :
            ///  SDL_SetWindowFullscreen(window,SDL_TRUE);
            ///  glViewport(0,0,screenRect.w,screenRect.h);
            ///break;

            ///case SDLK_g : SDL_SetWindowFullscreen(window,SDL_FALSE); break;
            case SDLK_UP :
              gameMoveF = -1;
            break;
            case SDLK_DOWN :
              gameMoveB = 1;
            break;
            case SDLK_LEFT :
              gameMoveL = -1;
            break;
            case SDLK_RIGHT :
              gameMoveR = 1;
            break;
            case SDLK_SPACE :
              game.changeState();
            break;
            default : break;

          } // end of key process
        } // end of keydown
        break;

        case SDL_KEYUP:
        {
          switch( event.key.keysym.sym )
          {
            case SDLK_UP :
              gameMoveF = 0;
            break;
            case SDLK_DOWN :
              gameMoveB = 0;
            break;
            case SDLK_LEFT :
              gameMoveL = 0;
            break;
            case SDLK_RIGHT :
              gameMoveR = 0;
            break;
            default : break;

          } // end of key process
        } // end of keydown

      } // end of event switch
    } // end of poll events


    gameMoveX += UPDATE*(gameMoveL+gameMoveR);
    gameMoveZ += UPDATE*(gameMoveF+gameMoveB);

    // now we draw game
    game.move(gameMoveX,gameMoveZ);
    game.update();
    game.draw();
    // swap the buffers
    SDL_GL_SwapWindow(window);

    gameMoveX = 0;
    gameMoveZ = 0;

  }
  // clean up the game scene
  //game.~Game();
}

//FUNCTION FROM JON MACEY
SDL_GLContext createOpenGLContext(SDL_Window *window)
{
  // Request an opegame 3.2 context first we setup our attributes, if you need any
  // more just add them here before the call to create the context
  // SDL doesn't have the ability to choose which profile at this time of writing,
  // but it should default to the core profile
  // for some reason we need this for mac but linux crashes on the latest nvidia drivers
  // under centos
  #ifdef DARWIN
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
  #endif
  // set multi sampling else we get really bad graphics that alias
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,4);
  // Turn on double buffering with a 24bit Z buffer.
  // You may need to change this to 16 or 32 for your system
  // on mac up to 32 will work but under linux centos build only 16
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  // enable double buffering (should be on by default)
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  //
  return SDL_GL_CreateContext(window);

}
