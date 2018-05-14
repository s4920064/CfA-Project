#include "Game.h"
#include "Ship.h"
#include "GameEnv.h"
#include <ngl/ShaderLib.h>
#include <ngl/NGLInit.h>
#include <ngl/Material.h>
#include <ngl/NGLStream.h>
#include <SDL2/SDL_rect.h>
#include <algorithm>


Game::Game()
{
  // set the clear color to dark grey
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);

  // set the time and score to zero
  m_time = 0;
  m_score = 0;

  ///-----------------The Camera------------------
  // values for the camera position
  ngl::Vec3 from(0,2.5,6);
  ngl::Vec3 to(0,0,0);
  ngl::Vec3 up(0,1,0);
  // load our camera
  m_camera= new ngl::Camera(from,to,up);
  // set the shape using FOV 45 Aspect Ratio based on Width and Height
  // The final two are near and far clipping planes of 0.5 and 10
  m_camera->setShape(45,720.0/576.0,0.05f,350.0f);
  ///---------------------------------------------

  ///------------------The Ship-------------------
  // initialize the shader program for drawing the ship
  initShaderProgram("Ship");

  // set the bounds rectangle
  m_moveBounds.w = 4;
  m_moveBounds.h = 6;
  m_moveBounds.x = -m_moveBounds.w/2;
  m_moveBounds.y = -m_moveBounds.h/2;

  // ship starting position
  ngl::Vec3 startPos(0,0,0);

  // ship mesh
  m_shipMesh = new ngl::Obj("models/buggy.obj");
  m_shipMesh->createVAO();

  // the ship object
  m_ship = new Ship(startPos, 3.0, m_moveBounds, m_shipMesh);
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
  initShaderProgram("GameEnv");

  // construct the game environment
  m_gameEnv = new GameEnv(/* "textures/Background.jpg" */);
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
  m_camera->setShape(45,(float)_w/_h,0.05,350);
}

void Game::draw()
{
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // ---Draw the Environment---
  m_gameEnv->draw(m_camera);

  // ---Draw the Ship---
  m_ship->draw(m_camera);

  // ---Draw the Projectiles---
  std::list <Projectile>::iterator i;
  std::list <Projectile>::iterator end=m_projectiles.end();

  for(i = m_projectiles.begin(); i != end; i++)
  {
    i->draw(m_camera);
    //std::cout<< "drawing a projectile!!!!\n";
  }
}

void Game::update()
{
  m_time++;

  m_score = m_time / 40;
  //std::cout<< m_score << "\n";

  if(m_time % 80 == 0)
  {
    int randX = (rand() % m_moveBounds.w) + m_moveBounds.x;
    Projectile *p = new Projectile( ngl::Vec3(randX,0,-100), 0.5f, m_projectileMesh);
    m_projectiles.push_back(*p);
    ++m_activeProjectiles;
    //std::cout<< p->m_state <<"\n";
  }

  std::list <Projectile>::iterator i;
  std::list <Projectile>::iterator end=m_projectiles.end();

  // iterater to update the projectiles
  for(i = m_projectiles.begin(); i != end; i++)
  {
    // update the projectile
    i->update(m_moveBounds.y);

    // if it is active
    if(i->isActive())
    {
      if(i->isThreat())
      {
        ngl::Vec3 shipPos = m_ship->getPos();
        float distance = sqrt( pow(i->m_position.m_x - shipPos.m_x, 2.0f) +
                               pow(i->m_position.m_z - shipPos.m_z, 2.0f) );
        // if there is a collision between the projectile and the ship
        if(distance < i->c_sphere + m_ship->c_sphere)
        {
          // if they are different states
          if(i->m_state != m_ship->m_state)
          {
            m_ship->m_lives -= 1.0;
          }
          else { m_ship->m_lives += 0.1; }
          m_projectiles.erase(i++);
        }
      }
    }
    else {m_projectiles.erase(i++);}
  }

  // update the size of the active projectiles
  m_activeProjectiles=m_projectiles.size();

  //std::cout<< m_ship->m_lives<<"\n";

}

void Game::initShaderProgram(const std::string &_name)
{
  // grab an instance of shader manager
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  // we are creating a shader called Texture
  shader->createShaderProgram(_name);
  // now we are going to create empty shaders for Frag and Vert
  shader->attachShader(_name + "Vertex", ngl::ShaderType::VERTEX);
  shader->attachShader(_name + "Fragment", ngl::ShaderType::FRAGMENT);
  // attach the source
  shader->loadShaderSource(_name + "Vertex", "shaders/" + _name + "Vertex.glsl");
  shader->loadShaderSource(_name + "Fragment", "shaders/" + _name + "Fragment.glsl");
  // compile the shaders
  shader->compileShader(_name + "Vertex");
  shader->compileShader(_name + "Fragment");
  // add them to the program
  shader->attachShaderToProgram(_name, _name + "Vertex");
  shader->attachShaderToProgram(_name, _name + "Fragment");
  // now we have associated this data we can link the shader
  shader->linkProgramObject(_name);
}

//void Game::keyEvent(SDL_KeyboardEvent &_event)
//{
//  switch (_event.keysym.sym)
//  {
//    case SDLK_UP :
//      m_ship->forward();
//    break;
//    case SDLK_DOWN :
//      m_ship->backward();
//    break;
//    case SDLK_LEFT :
//      m_ship->left();
//    break;
//    case SDLK_RIGHT :
//      m_ship->right();
//    break;
//    case SDLK_SPACE :
//      m_ship->changeState();
//    break;
//    default:
//    break;
//  }
//}
