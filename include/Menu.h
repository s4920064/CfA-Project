#ifndef _MENU_H_
#define _MENU_H_
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

//template<size_t SIZE, class T> inline size_t array_size(T (&arr)[SIZE]) {
//    return SIZE;
//}
//this passing of functions from source https://stackoverflow.com/questions/5859455/how-to-implement-a-generic-button-class-that-executes-different-functions-on-cli
typedef void(*eventFunction)();

class Button
{
public:
    SDL_Texture *buttonTexture;
    SDL_Rect borderRect;                        //rectangle border of the button. In public because we will use these values to blit the button onto the final menu texture and to determine if button is clicked in main event loop
    SDL_Rect textRect;

    Button(const char *label, SDL_Renderer *renderer);
    ~Button();
    bool setPosition(int _x, int _y, bool centered);           //set position of button (will edit borderRect)
    bool isInside(int mouseX, int mouseY);       //check if mouse position is within button borders

private:
    eventFunction m_functPtr;
    int buttonSpacing;
    SDL_Surface *buttonSurface = NULL;                 //pixel info of the button which will later be converted to a texture (optimized)
    SDL_Surface *textSurface;                   //pixel info of the label text
    Uint32 buttonColor;                         //fill color of the button
    SDL_Color textColor;                        //color of the button label text
    TTF_Font *textFont;                         //font of the button label text
};

class Menu
{
public:
    //Button buttonList[];                // current list of buttons available to the user
    SDL_Renderer *m_Renderer = NULL;
    int m_winWidth;                   // bad to have the window width stated twice in the program?
    int m_winHeight;                  // bad to have the window height stated twice in the program?

    Menu(SDL_Window* window, SDL_Rect rect);
    ~Menu();
    void MainMenu(Button playButton);
    void Instructions(Button backButton);
    void HighScores();
    void Game(SDL_Rect rect, bool quit);

private:
    SDL_Window *m_window;             // bad to have the window stated twice in the program?
    SDL_Texture *UITexture = NULL;
};

#endif
