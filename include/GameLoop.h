#ifndef _GAMELOOP_H__
#define _GAMELOOP_H__
#include <SDL2/SDL.h>
#include <cstdlib>
#include <iostream>

/// @brief function to quit SDL with error message
/// @param[in] _msg the error message to send
void SDLErrorExit(const std::string &_msg);

void PlayGame(SDL_Window *window, SDL_Rect screenRect, bool *quit);

/// @brief initialize SDL OpenGL context
SDL_GLContext createOpenGLContext( SDL_Window *window);

#endif
