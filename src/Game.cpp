#include "Game.h"
#include <ngl/ShaderLib.h>
#include <ngl/NGLInit.h>
#include <ngl/Material.h>
#include <ngl/NGLStream.h>
#include <SDL2/SDL_rect.h>
#include <algorithm>


Game::Game()
{
  // set the clear color to grey
  glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);

  ///-----------------The Camera------------------
  // values for the camera position
  ngl::Vec3 from(0,5,10);
  ngl::Vec3 to(0,0,0);
  ngl::Vec3 up(0,3,0);
  m_view=ngl::lookAt(from,to,up);
  // load our camera
  m_camera= new ngl::Camera(from,to,up);
  // set the shape using FOV 45 Aspect Ratio based on Width and Height
  // The final two are near and far clipping planes of 0.5 and 10
  m_camera->setShape(45,(float)720.0/576.0,0.05,350);
  ///---------------------------------------------

  ///------------------The Ship-------------------
  // initialize the shader program for drawing the ship
  initShaderProgram("Ship");

  // set the bounds rectangle
  m_moveBounds.w = 5.0;
  m_moveBounds.h = 5.0;
  m_moveBounds.x = -m_moveBounds.w/2.0;
  m_moveBounds.y = m_moveBounds.y/2.0;

  // ship starting position
  ngl::Vec3 startPos(0,0,0);

  // ship mesh
  m_shipMesh = new ngl::Obj("models/buggy.obj");
  m_shipMesh->createVAO();

  // the ship object
  m_ship = new Ship(startPos, 3.0, &m_moveBounds, m_shipMesh);
  ///---------------------------------------------

  ///---------------The Projectiles---------------
  // initialize the shader program for drawing projectiles
  initShaderProgram("Projectile");

  // the mesh
  m_projectileMesh = new ngl::Obj("models/buggy.obj");
  m_projectileMesh->createVAO();

  // set the number of active projectiles to zero
  m_activeProjectiles = 0;
  ///---------------------------------------------

  ///-------------The Game Environment------------
  // initialize the shader program for drawing the game environment
  initShaderProgram("Background");

  // construct the game environment
  m_gameEnv = new GameEnv("textures/Background.png");
  ///---------------------------------------------


}

Game::~Game()
{
  std::cout<<"Shutting down Game, removing VAO's and Shaders\n";
  // free the slaves, i mean the memory
  delete m_camera;
  delete m_shipMesh;
  delete m_projectileMesh;
  delete m_ship;
  delete m_gameEnv;
}

void Game::resize(int _w, int _h)
{
  glViewport(0,0,_w,_h);
  // now set the camera size values as the screen size has changed
  m_cam->setShape(45,(float)_w/_h,0.05,350);
}

void Game::draw()
{
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // grab an instance of shader manager
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();

  // ---Draw the Environment---
  // use the Background shader program to draw the game environment
  (*shader)["Background"]->use();
  // draw the game envrironment
  m_gameEnv.draw(m_camera);

  // ---Draw the Ship---
  // use the Ship shader program to draw the game environment
  (*shader)["Ship"]->use();
  // draw the ship
  m_ship.draw(m_camera);

  // ---Draw the Projectiles---
  // use the Projectiles shader program to draw the projectiles
  (*shader)["Projectile"]->use();
  // draw the projectiles
  std::list <Projectile *>::iterator start=m_projectiles.begin();
  std::list <Projectile *>::iterator end=m_projectiles.end();
  // use std::for_each and pass in a member function to draw
  std::for_each(start,end,std::mem_fun(&Projectile::draw));
}

void Game::update()
{
  std::list <Projectile *>::iterator i=m_projectiles.begin();
  std::list <Projectile *>::iterator end=m_projectiles.end();

  // iterater to update the projectiles
  for(i;  i != end; i++)
  {
    // update the projectile
    i->update(m_moveBounds);

    // if it is active
    if(i->isActive())
    {
      if(i->isThreat())
      {
        ngl::Vec3 shipPos = m_ship.getPos();
        float distance = sqrt( pow(i->m_position->m_x - shipPos.m_x, 2)
                               pow(i->m_position->m_z - shipPos.m_z, 2) );
        // if there is a collision between the projectile and the ship
        if(distance < i->c_sphere + m_ship.c_sphere)
        {
          // if they are different states
          if(i->m_state != m_ship.m_state)
          {
            m_ship.m_lives -= 1.0;
          }
          else { m_ship.m_lives += 0.1; }
          m_projectiles.erase(i++);
        }
      }
    }
    else {m_projectiles.erase(i++);}
  }

  // update the size of the active projectiles
  m_activeProjectiles=m_projectiles.size();
}

void Game::initShaderProgram(const std::string &_name)
{
  // we are creating a shader called Texture
  shader->createShaderProgram(_name);
  // now we are going to create empty shaders for Frag and Vert
  shader->attachShader("%sVertex" % (_name), ngl::ShaderType::VERTEX);
  shader->attachShader("%sFragment" % (_name), ngl::ShaderType::FRAGMENT);
  // attach the source
  shader->loadShaderSource("%sVertex" % (_name),"shaders/%sVertex.glsl" % (_name));
  shader->loadShaderSource("%sFragment" % (_name),"shaders/%sFragment.glsl" % (_name));
  // compile the shaders
  shader->compileShader("%sVertex" % (_name));
  shader->compileShader("%sFragment" % (_name));
  // add them to the program
  shader->attachShaderToProgram(_name,"%sVertex" % (_name));
  shader->attachShaderToProgram(_name,"%sFragment" % (_name));
  // now we have associated this data we can link the shader
  shader->linkProgramObject(_name);
}

void Game::keyEvent(SDL_KeyboardEvent &_event)
{
  switch (_event.keysym.sym)
  {
    case SDLK_UP :
      m_ship->forward();
    break;
    case SDLK_DOWN :
      m_ship->backward();
    break;
    case SDLK_LEFT :
      m_ship->left();
    break;
    case SDLK_RIGHT :
      m_ship->right();
    break;
    default:
    break;
  }
}
