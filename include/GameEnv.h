#ifndef GAMEENV_H__
#define GAMEENV_H__
#include <SDL2/SDL.h>

//data for the object Grid
class Grid
{
public:
  Grid();
  ~Grid();
private:
  // the area within the grid where the player can move
  SDL_Rect m_movementArea;
};

#endif
