#ifndef _MENU_H_
#define _MENU_H_
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

//this passing of functions from source https://stackoverflow.com/questions/5859455/how-to-implement-a-generic-button-class-that-executes-different-functions-on-cli
//typedef void(*eventFunction)();

class Button
{
public:
  //rectangle border of the button. In public because we will use these values to blit the button onto the final menu texture and to determine if button is clicked in main event loop
  SDL_Rect m_borderRect;
  // rectangle border of the bare text
  SDL_Rect m_textRect;

  Button();
  Button(const char *_label, int _size);
  Button(const char *_label, SDL_Renderer *_renderer, int _size);
  ~Button();
  void setTexture(SDL_Renderer *_renderer);
  inline SDL_Texture *getTexture() { return m_buttonTexture; }
  inline SDL_Surface *getSurface() { return m_textSurface; }
  inline TTF_Font *getFont() { return m_textFont; }
  void setPosition(int _x, int _y, bool _centered);   //set position of button (will edit borderRect)
  bool isInside(int _mouseX, int _mouseY);            //check if mouse position is within button borders

private:
  // eventFunction m_functPtr;         //currently not using function pointers
  // the spacing between the text and the button border
  int m_buttonSpacing;
  // pixel info of the label text
  SDL_Surface *m_textSurface;
  // the texture that will be used in the menus
  SDL_Texture *m_buttonTexture;
  // color of the button label text
  SDL_Color m_textColor;
  // font of the button label text
  TTF_Font *m_textFont;
};

// Menu base class
class Menu
{
public:
//  Menu();
//  ~Menu();
protected:
  SDL_Color m_backColor = {11,11,11,225};                // background color
  SDL_Color m_buttonColor = {11, 11, 11, 255};
};

// Main Menu class
class MainMenu: public Menu
{
public:
  MainMenu(SDL_Renderer *_renderer, SDL_Rect _windowRect);
  ~MainMenu();
  void render(SDL_Renderer *_renderer);
  void handleButtonPresses(int _mouseX, int _mouseY);

  Button m_playButton;
// Button m_instructButton;
//  Button highscoresButton;
//  Button exitButton;
};

#endif
