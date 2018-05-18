/// \file Game.h
/// \brief encapsulates a 4d Homogenous Point / Vector object
/// \author Emma Koo
/// \version 3.0
/// \date 28/9/09 Updated to NCCA Coding standard
/// Revision History :
/// Initial Version 24/11/04
/// \todo lots of tidying up and code optimizations

#ifndef GAME_H__
#define GAME_H__
#include <ngl/Camera.h>
#include <ngl/ShaderLib.h>
#include <list>
#include <SDL.h>
#include "Ship.h"
#include "Text.h"
#include "Projectile.h"
#include "GameEnv.h"

const static int SCORELENGTH = 5;

struct Score
{
  unsigned int m_score;
  int m_scoreArray[SCORELENGTH];
  SDL_Rect m_scoreRect;
};

bool intToArray(int _integer, int _length, int _array[SCORELENGTH]);

class Game
{
  public :
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief ctor this will have a valid OpenGL context so we can create gl stuff
    //----------------------------------------------------------------------------------------------------------------------
    Game(int _h);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief dtor used to remove any NGL stuff created
    //----------------------------------------------------------------------------------------------------------------------
    ~Game();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief resize our screen and set the camera aspect ratio
    /// @param[in] _w the new width
    /// @param[in] _h the new height
    //----------------------------------------------------------------------------------------------------------------------
    void resize(int _h);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief draw the scene
    //----------------------------------------------------------------------------------------------------------------------
    void draw();

    // method to move the ship
    void move(float _x, float _z);

    // reset the ship position to the center
    inline void resetPosition(){ m_ship->setX(0); m_ship->setZ(0); }

    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called every time a mouse is moved
    /// @param _event the SDL mouse event structure containing all mouse info
    //----------------------------------------------------------------------------------------------------------------------
    void mouseMoveEvent (const SDL_MouseMotionEvent &_event);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called every time a mouse button is presses
    /// @param _event the SDL mouse event structure containing all mouse info
    //----------------------------------------------------------------------------------------------------------------------
    void mousePressEvent (const SDL_MouseButtonEvent &_event);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called everytime the mouse button is released
    /// @param _event the SDL mouse event structure containing all mouse info
    //----------------------------------------------------------------------------------------------------------------------
    void mouseReleaseEvent (const SDL_MouseButtonEvent &_event);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called everytime the mouse wheel is moved
    /// @param _event the SDL mouse event structure containing all mouse info
    //----------------------------------------------------------------------------------------------------------------------
    void wheelEvent(const SDL_MouseWheelEvent &_event);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is to update the scene
    //----------------------------------------------------------------------------------------------------------------------
    void update();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is to update the scene
    //----------------------------------------------------------------------------------------------------------------------
    void move(int _x,int _z);

    inline void changeState() {m_ship->changeState();}

    Score         m_score;

    unsigned int  m_time;

    float         m_lives;

  private :
    // initialize a shader program so that it can be used to draw
    //void initShaderProgram(const std::string &_name);

    // Utility function for loading up a 2D texture
    void initTexture(const GLuint&, GLuint &, const char *);

    // initilize the framebuffer objects
    void initFBO();

    void render_text(const char *text, float x, float y, float sx, float sy);

    int getSpawnCycle(int initialPeriod, int randomImprecision);

    // Keep track of whether the FBO needs to be recreated
    bool          m_isFBODirty = true;

    // the width and height of the window
    int           m_width,
                  m_height;

    // the number of active projectiles
    int           m_activeMissiles,
                  m_activeObstacles,
                  m_activeBonuses,
                  m_activeProjectiles;

    // our camera
    ngl::Camera  *m_camera;

    // the meshes for all the objects in the scene
    ngl::Obj     *m_shipMesh,
                 *m_missileMesh,
                 *m_obstacleMesh,
                 *m_bonusMesh;

    // the list of active projectiles
    std::list <Projectile *> m_projectiles;

    // the game environment
    GameEnv       *m_gameEnv;

    // the ship
    Ship          *m_ship;

    // the ship movement bounds rectangle
    SDL_Rect      m_moveBounds;

    ngl::Vec2     m_textSurface;

    // the IDs used for the frame buffer objects and their associated textures
    GLuint        m_fboId,
                  m_fboGameTexId,
                  m_fboGameDepthId,
                  m_fboTextTexId,
                  m_fboNumTexId,
                  m_fboMaskTexId,
                  m_fboLifeTexId,
                  m_fboGameOverTexId;

//    Text *m_text;
    //QFont *m_font = new QFont("Arial",16);

};


#endif
