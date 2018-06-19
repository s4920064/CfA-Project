#include "Game.h"
#include "Ship.h"
#include "GameEnv.h"
#include "Menu.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ngl/ShaderLib.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/NGLInit.h>
#include <ngl/Material.h>
#include <ngl/NGLStream.h>
#include <SDL2/SDL_rect.h>
#include <algorithm>
#include <SDL2/SDL_ttf.h>

Game::Game(int _h)
{
  // set width and height
  m_width = _h*1.5;
  m_height = _h;

  // set the time and score to zero, and lives to full
  m_time = 0;
  m_score.m_score = 0;
  m_lives = 3.0;

  // set the clear color to dark grey
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

  // Enable 2D texturing
  glEnable(GL_TEXTURE_2D);

  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);

  //glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // enable multisampling for smoother drawing
  glEnable(GL_MULTISAMPLE);

  // grab an instance of the shader manager
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();

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

  ///----------The Screen-Oriented Plane----------
  shader->loadShader("Game",
                     "shaders/GameVertex.glsl",
                     "shaders/GameFragment.glsl");


  // Create a screen oriented plane
  ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();
  prim->createTrianglePlane("plane",2,2,1,1,ngl::Vec3(0,1,0));
  ///---------------------------------------------

  // load the shader that will be used to draw the ship and projectiles
  shader->loadShader("Diffuse",
                     "shaders/ShipVertex.glsl",
                     "shaders/ShipFragment.glsl");

  ///------------------The Ship-------------------
  // set the bounds rectangle
  m_moveBounds.w = 4;
  m_moveBounds.h = 6;
  m_moveBounds.x = -m_moveBounds.w/2;
  m_moveBounds.y = -m_moveBounds.h/2;

  // ship starting position
  ngl::Vec3 startPos(0,0,0);

  // ship mesh
  m_shipMesh = new ngl::Obj("models/Ship.obj");
  m_shipMesh->createVAO();

  // the ship object
  m_ship = new Ship(startPos, m_moveBounds, m_shipMesh);
  ///---------------------------------------------

  ///---------------The Projectiles---------------
//  shader->loadShader("Projectile",
//                     "shaders/ProjectileVertex.glsl",
//                     "shaders/ProjectileFragment.glsl");

  // load the meshes
  m_missileMesh = new ngl::Obj("models/Missile.obj");
  m_missileMesh->createVAO();
  m_obstacleMesh = new ngl::Obj("models/Obstacle.obj");
  m_obstacleMesh->createVAO();
  m_bonusMesh = new ngl::Obj("models/Bonus.obj");
  m_bonusMesh->createVAO();

  // set the number of active projectiles to zero
  m_activeMissiles = 0;
  m_activeObstacles = 0;
  m_activeBonuses = 0;
  ///---------------------------------------------

  ///-------------The Game Environment------------
  // initialize the shader program for drawing the game environment
  //initShaderProgram("GameEnv");
  shader->loadShader("GameEnv",
                     "shaders/GameEnvVertex.glsl",
                     "shaders/GameEnvFragment.glsl");

  // construct the game environment
  m_gameEnv = new GameEnv(/* "textures/Background.jpg" */);
  ///---------------------------------------------

//  m_text = new Text("font/game_over.ttf",100);
//  m_text->setColour(1.0,0.0,0.0);

  //m_text.resetnew ngl::Text(*m_font);
  //m_text->setScreenSize(m_width,m_height);
  //m_text->setColour(1,1,1);

}

Game::~Game()
{
  std::cout<<"Shutting down Game, removing VAO's and Shaders\n";
  // free the slaves, i mean the memory
  delete m_camera;
  delete m_shipMesh;
  delete m_missileMesh;
  delete m_obstacleMesh;
  delete m_bonusMesh;
  delete m_ship;
  delete m_gameEnv;
  //delete m_text;
}

void Game::resize(int _h)
{
  // set the width and height to the new values,
  // keeping a constant w/h ratio
  m_width = int(float(_h)*1.5f);
  m_height = _h;

//  m_text->setScreenSize(m_width,m_height);

  glViewport(0,0,m_width,m_height);
  // now set the camera size values as the screen size has changed
  m_camera->setShape(45,(float)m_width/m_height,0.05,350);

  // the FBO is dirty
  m_isFBODirty = true;
}

void Game::draw()
{
  // Check if the FBO needs to be recreated. This occurs after a resize.
  if (m_isFBODirty) {
      initFBO();
      m_isFBODirty = false;
  }

  // Bind the FBO to specify an alternative render target
  glBindFramebuffer(GL_FRAMEBUFFER, m_fboId);


  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Set up the viewport
  glViewport(0,0,m_width,m_height);

  /// ---Draw the Environment---
  m_gameEnv->draw(m_camera);

  /// ---Draw the Ship---
  m_ship->draw(m_camera);

  /// ---Draw the Projectiles---
  std::list <Projectile *>::iterator p;
  std::list <Projectile *>::iterator end=m_projectiles.end();

  for(p = m_projectiles.begin(); p != end; p++)
  {
    (*p)->draw(m_camera);
    //std::cout<< "drawing a projectile!!!!\n";
  }

  /// ---Draw the Final Image (on screen-oriented plane)--

  // Unbind our FBO (set to default)
  glBindFramebuffer(GL_FRAMEBUFFER,0);

  // Set background colour
  glClearColor(0, 0, 0, 0);

  // Clear the screen
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_width,m_height);

  // Game texture bind
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, m_fboGameTexId);

  // Depth texture bind
  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, m_fboGameDepthId);

  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_2D, m_fboMaskTexId);

  glActiveTexture(GL_TEXTURE6);
  glBindTexture(GL_TEXTURE_2D, m_fboTextTexId);

  glActiveTexture(GL_TEXTURE7);
  glBindTexture(GL_TEXTURE_2D, m_fboNumTexId);

  glActiveTexture(GL_TEXTURE8);
  glBindTexture(GL_TEXTURE_2D, m_fboLifeTexId);

  glActiveTexture(GL_TEXTURE9);
  glBindTexture(GL_TEXTURE_2D, m_fboGameOverTexId);

  // grab an instance of the shader manager
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  // use the Game shader program for this draw
  (*shader)["Game"]->use();
  GLuint pid = shader->getProgramID("Game");

  // send the game texture and depth to the GPU
  glUniform1i(glGetUniformLocation(pid, "gameTex"), 3);
  glUniform1i(glGetUniformLocation(pid, "depthTex"), 4);
  glUniform1i(glGetUniformLocation(pid, "textTex"), 6);
  glUniform1i(glGetUniformLocation(pid, "maskTex"), 5);
  glUniform1i(glGetUniformLocation(pid, "numTex"), 7);
  glUniform1i(glGetUniformLocation(pid, "lifeTex"), 8);
  glUniform1i(glGetUniformLocation(pid, "gameOverTex"), 9);
  glUniform2f(glGetUniformLocation(pid, "windowSize"), m_width, m_height);
  glUniform2f(glGetUniformLocation(pid, "scorePlacement"), m_score.m_scoreRect.x, m_score.m_scoreRect.y);
  glUniform2f(glGetUniformLocation(pid, "numDimensions"), m_score.m_scoreRect.w, m_score.m_scoreRect.h);
  glUniform1i(glGetUniformLocation(pid, "scoreLength"), SCORELENGTH);
  glUniform1iv(glGetUniformLocation(pid, "score"), SCORELENGTH, m_score.m_scoreArray);
  glUniform1f(glGetUniformLocation(pid, "lives"), m_lives);

  // set the MVP for the plane
  glm::mat4 MVP = glm::rotate(glm::mat4(1.0f), glm::pi<float>() * 0.5f, glm::vec3(1.0f,0.0f,0.0f));
  glUniformMatrix4fv(glGetUniformLocation(pid, "MVP"), 1, false, glm::value_ptr(MVP));

  // Grab and instance of the VAO primitives path
  ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();

  // draw the plane
  prim->draw("plane");

  // bind the plane texture
  glBindTexture(GL_TEXTURE_2D, 0);

}

// this function is based off of the algorithm by Broam in https://stackoverflow.com/questions/1860983/convert-integer-to-array
bool intToArray(int _integer, int _length, int _array[SCORELENGTH])
{
  if(_length==0)
  {
    return true;
  }
  else
  {
    _array[_length-1] = _integer%10;
    intToArray(_integer/10,_length-1, _array);
  }
}

// calculates the loop cycle period where a projectile will spawn
// to make the spawning a little more unpredictable, give it a random range
int Game::getSpawnCycle(int initialPeriod, int initialRandRange)
{
  // a random value between the positive and negative initial random range
  int initialRand = rand() % (initialRandRange*2) - initialRandRange;
  int spawnPeriod = int((initialPeriod+initialRand)/(float(m_time)/300));
  if(spawnPeriod<initialRandRange+10)
  {
    return initialRandRange+10;
  }
  else
  {
    return spawnPeriod;
  }
}

void Game::update()
{
  if(m_lives>0)
  {
    m_time++;

    m_score.m_score = m_time / 40;
    if(!intToArray(m_score.m_score,SCORELENGTH,m_score.m_scoreArray))
    {
      std::cout<<"Recursive intToArray() function failed!\n";
    }

    //std::cout<< m_score.m_scoreArray[0]<<m_score.m_scoreArray[1]<<m_score.m_scoreArray[2]<< "\n";

    ///---------------Spawn Projectiles-------------

    /// Spawn Missiles
    // every ~80(+-15) cycles
    if(m_time % getSpawnCycle(80,15) == 0)
    {
      // generate a random floating point value within the horizontal movebounds area
      float randX = float(rand() % (m_moveBounds.w*10))/10 + m_moveBounds.x;
      // create a new missile at that point with speed 0.5
      Missile *m = new Missile( ngl::Vec3(randX,0,-100), 0.5f, m_missileMesh);
      // push it onto the list of projectiles
      m_projectiles.push_back(m);
      // increment the number of active projectiles
      ++m_activeProjectiles;
    }

    /// Spawn Obstacles
    // every ~160 cycles
    if(m_time % getSpawnCycle(160,30) == 0)
    {
      // generate a random floating point value within the horizontal movebounds area
      float randX = float(rand() % (m_moveBounds.w*10))/10 + m_moveBounds.x;
      // create a new obstacle at that point with speed 0.5
      Obstacle *m = new Obstacle( ngl::Vec3(randX,0,-100), 0.5f, m_obstacleMesh);
      // push it onto the list of projectiles
      m_projectiles.push_back(m);
      // increment the number of active projectiles
      ++m_activeProjectiles;
    }

    /// Spawn Bonuses
    // every 200 cycles
    if(m_time % getSpawnCycle(200,30) == 0)
    {
      // generate a random floating point value within the horizontal movebounds area
      float randX = float(rand() % (m_moveBounds.w*10))/10 + m_moveBounds.x;
      // create a new bonus at that point with speed 0.5
      Bonus *b = new Bonus( ngl::Vec3(randX,0,-100), 0.5f, m_bonusMesh);
      // push it onto the list of projectiles
      m_projectiles.push_back(b);
      // increment the number of active projectiles
      ++m_activeProjectiles;
    }

    ///---------------------------------------------

    ///-------Update Projectiles and Collision------
    // iterator for the list of missiles
    std::list <Projectile *>::iterator p;
    std::list <Projectile *>::iterator end=m_projectiles.end();

    // for every active projectile p
    for(p = m_projectiles.begin(); p != end; p++)
    {
      // update the projectile
      (*p)->update(m_moveBounds.y, m_ship->c_sphere);

      // if it is active
      if((*p)->isActive())
      {
        // if it is a threat
        if((*p)->isThreat())
        {
          // distance between the ship and the projectile in question
          float distance = sqrt( pow((*p)->m_position.m_x - m_ship->getPos().m_x, 2.0f) +
                                 pow((*p)->m_position.m_z - m_ship->getPos().m_z, 2.0f) );
          // if there is a collision between the projectile and the ship
          if(distance < (*p)->c_sphere + m_ship->c_sphere)
          {

            // if the projectile is a bonus
            if(strcmp((*p)->getType(),"bonus")==0)
            {
              // if they are the same state
              if((*p)->m_state == m_ship->m_state)
              {
                // then add one full life
                m_lives += 1.0;

                // start the ship health timer
                m_ship->setEffect(10,false);

                // then erase the projectile from the list
                m_projectiles.erase(p++);
              }
            }

            else
            {
              // if the projectile is a missile
              if(strcmp((*p)->getType(),"missile")==0)
              {
                // if they are different states
                if((*p)->m_state != m_ship->m_state)
                {
                  // then remove one full life
                  m_lives -= 1.0;

                  // start the ship damage timer
                  m_ship->setEffect(40,true);
                }
                // if they are of the same state then add a bit of life
                else
                {
                  m_lives += 0.1;
                  // start the ship health timer
                  m_ship->setEffect(10,false);
                }
              }

              // if the projectile is an obstacle
              if(strcmp((*p)->getType(),"obstacle")==0)
              {
                // then remove one full life
                m_lives -= 1.0;

                // start the ship damage timer
                m_ship->setEffect(40,true);
              }

              // then erase the projectile from the list
              m_projectiles.erase(p++);
            }

          }
        }
      }
      // if it is not active, erase it from the list
      else {m_projectiles.erase(p++);}
    }

    if(m_lives>3)
    {
      m_lives = 3;
    }

    // update the size of the active projectiles
    m_activeProjectiles=m_projectiles.size();
    ///---------------------------------------------

    ///----------------Update GameEnv---------------
    m_gameEnv->update(0.2);
    ///---------------------------------------------

    ///-----------------Update Ship-----------------
    m_ship->update();
    ///---------------------------------------------
  }
}

void Game::move(float _x, float _z)
{

  // if the new position won't be past the top and bottom side of the bounds rect
  float shipZ = m_ship->getPos().m_z;
  if(shipZ + _z > m_moveBounds.y && shipZ + _z < m_moveBounds.y + m_moveBounds.h )
  { m_ship->setZ(shipZ+_z); }

  // if the new position won't be past the left and right side of the bounds rect
  float shipX = m_ship->getPos().m_x;
  if(shipX + _x > m_moveBounds.x && shipX + _x < m_moveBounds.x + m_moveBounds.w )
  { m_ship->setX(shipX+_x); }

}

// following code based off of the exercises by Richard Southern
void Game::initFBO()
{
  // First delete the FBO if it has been created previously
  glBindFramebuffer(GL_FRAMEBUFFER, m_fboId);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER)==GL_FRAMEBUFFER_COMPLETE)
  {
      glDeleteTextures(1, &m_fboGameTexId);
      glDeleteTextures(1, &m_fboGameDepthId);
      glDeleteFramebuffers(1, &m_fboId);
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // The game pass is rendered to a texture buffer
  glGenTextures(1, &m_fboGameTexId);
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, m_fboGameTexId);
  glTexImage2D(GL_TEXTURE_2D,
               0,
               GL_RGBA,
               m_width,
               m_height,
               0,
               GL_RGBA,
               GL_UNSIGNED_BYTE,
               NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  // The depth pass is rendered to a texture buffer too
  glGenTextures(1, &m_fboGameDepthId);
  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, m_fboGameDepthId);
  glTexImage2D(GL_TEXTURE_2D,
               0,
               GL_DEPTH_COMPONENT,
               m_width,
               m_height,
               0,
               GL_DEPTH_COMPONENT,
               GL_UNSIGNED_BYTE,
               NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  ///-----------------Generate GameScreen UI Surface-----------------
  // create a surface the size of the screen and fill it with a background color
  SDL_Surface* screenSurface = SDL_CreateRGBSurface(0, m_width, m_height, 32, 0,0,0,1);
  SDL_FillRect(screenSurface,NULL,SDL_MapRGB(screenSurface->format, 11, 11, 11));

  ngl::Vec2 gameSize = ngl::Vec2(0.55,0.9);
  float border = 0.05f;

  SDL_Rect gameViewport;
  gameViewport.w = int(gameSize.m_x*m_width);
  gameViewport.h = int(gameSize.m_y*m_height);
  gameViewport.x = int(border*m_width);
  gameViewport.y = int(border*m_height);

  // create a surface the size of the game viewport (this will be a mask) and fill it black
  SDL_Surface* maskSurface = SDL_CreateRGBSurface(0, gameViewport.w, gameViewport.h, 32, 0,0,0,1);
  SDL_FillRect(maskSurface,NULL,SDL_MapRGB(maskSurface->format, 0, 0, 0));

  // create some text from our Button class and save the surface
  Button scoreText = Button("SCORE", 100);
  SDL_Surface *scoreSurface = scoreText.getSurface();

  // set the positions of the different text surfaces within the area of the screen
  scoreText.setPosition(m_width-m_width/5, m_height/7, true);

  // blit the text surfaces onto our screen-sized surface
  SDL_BlitSurface(maskSurface, NULL, screenSurface, &gameViewport);
  SDL_BlitSurface(scoreSurface, NULL, screenSurface, &scoreText.m_textRect);
  ///-------------------------------------------------------

  ///-----------------Text-----------------
  // render the SDL_Surface containing our text to a texture buffer
  glGenTextures(1, &m_fboTextTexId);
  glActiveTexture(GL_TEXTURE6);
  glBindTexture(GL_TEXTURE_2D, m_fboTextTexId);
  glTexImage2D(GL_TEXTURE_2D,
               0,
               GL_RGBA,
               screenSurface->w,
               screenSurface->h,
               0,
               GL_BGRA_EXT,
               GL_UNSIGNED_BYTE,
               screenSurface->pixels);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);
  ///---------------------------------------

  ///-----------------Mask-----------------
  SDL_FillRect(screenSurface,NULL,SDL_MapRGB(screenSurface->format, 0, 0, 0));
  SDL_FillRect(maskSurface,NULL,SDL_MapRGB(maskSurface->format, 255, 255, 255));
  SDL_BlitSurface(maskSurface, NULL, screenSurface, &gameViewport);

  // render the SDL_Surface containing our mask to a texture buffer
  glGenTextures(1, &m_fboMaskTexId);
  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_2D, m_fboMaskTexId);
  glTexImage2D(GL_TEXTURE_2D,
               0,
               GL_RGBA,
               screenSurface->w,
               screenSurface->h,
               0,
               GL_BGRA_EXT,
               GL_UNSIGNED_BYTE,
               screenSurface->pixels);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);
  ///---------------------------------------

  ///-----------------Life Bars-----------------
  // fill the screenSurface black
  SDL_FillRect(screenSurface,NULL,SDL_MapRGB(screenSurface->format, 0, 0, 0));

  // create a surface for every life and blit it to the screenSurface
  for(int i; i<m_lives; i++)
  {
    SDL_Rect lifeRect;
    lifeRect.w = 50;
    lifeRect.h = 30;
    lifeRect.x = float(m_width)-float(m_width)/4.3f;
    // each position will be lower than the last
    lifeRect.y = m_height/2+(lifeRect.h+50)*i-lifeRect.h;
    // create the SDL_Surface for the life bar and fill it green
    SDL_Surface* lifeSurface = SDL_CreateRGBSurface(0, lifeRect.w, lifeRect.h, 32, 0,0,0,1);
    SDL_FillRect(lifeSurface,NULL,SDL_MapRGB(lifeSurface->format, 0, 255, 0));
    // blit it to the screenSurface
    SDL_BlitSurface(lifeSurface, NULL, screenSurface, &lifeRect);
    // free the lifeSurface, since it was created locally
    SDL_FreeSurface(lifeSurface);
  }

  // render the SDL_Surface containing our life bars to a texture buffer
  glGenTextures(1, &m_fboLifeTexId);
  glActiveTexture(GL_TEXTURE8);
  glBindTexture(GL_TEXTURE_2D, m_fboLifeTexId);
  glTexImage2D(GL_TEXTURE_2D,
               0,
               GL_RGBA,
               screenSurface->w,
               screenSurface->h,
               0,
               GL_BGRA_EXT,
               GL_UNSIGNED_BYTE,
               screenSurface->pixels);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);
  ///---------------------------------------

  ///-----------------Game Over-----------------
  // fill our screen with black
  SDL_FillRect(screenSurface,NULL,SDL_MapRGB(screenSurface->format, 0, 0, 0));

  // set the game over text to a surface
  Button gameOverButton = Button("GAME OVER",200);
  SDL_Surface *gameOverSurface = gameOverButton.getSurface();
  gameOverButton.setPosition(m_width/2,m_height/2,true);

  // blit it to the screenSurface
  SDL_BlitSurface(gameOverSurface, NULL, screenSurface, &gameOverButton.m_borderRect);
  // free the lifeSurface, since it was created locally
  SDL_FreeSurface(gameOverSurface);

  // render the SDL_Surface containing our mask to a texture buffer
  glGenTextures(1, &m_fboGameOverTexId);
  glActiveTexture(GL_TEXTURE9);
  glBindTexture(GL_TEXTURE_2D, m_fboGameOverTexId);
  glTexImage2D(GL_TEXTURE_2D,
               0,
               GL_RGBA,
               screenSurface->w,
               screenSurface->h,
               0,
               GL_BGRA_EXT,
               GL_UNSIGNED_BYTE,
               screenSurface->pixels);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);
  ///---------------------------------------


  ///-----------------Score Numbers-----------------
  // create some text displaying all digits and store it in an SDL_Surface
  Button numbers = Button(" 0  1  2  3  4  5  6  7  8  9 ", 100);
  SDL_Surface *numberSurface = numbers.getSurface();

  // set the score number placement
  m_score.m_scoreRect.x = scoreText.m_textRect.x;
  m_score.m_scoreRect.y = scoreText.m_textRect.y+m_height/10;

  // set the position of the numbers to the top-left corner
  numbers.setPosition(0, 0, false);

  // give the screen surface the background color
  SDL_FillRect(screenSurface,NULL,SDL_MapRGB(screenSurface->format, 11, 11, 11));
  SDL_BlitSurface(numberSurface, NULL, screenSurface, &numbers.m_textRect);

  // set the score number dimensions
  TTF_SizeText(numbers.getFont(),"0  ", &m_score.m_scoreRect.w, &m_score.m_scoreRect.h);

  // render the SDL_Surface containing our text to a texture buffer
  glGenTextures(1, &m_fboNumTexId);
  glActiveTexture(GL_TEXTURE7);
  glBindTexture(GL_TEXTURE_2D, m_fboNumTexId);
  glTexImage2D(GL_TEXTURE_2D,
               0,
               GL_RGBA,
               screenSurface->w,
               screenSurface->h,
               0,
               GL_BGRA_EXT,
               GL_UNSIGNED_BYTE,
               screenSurface->pixels);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);
  ///---------------------------------------

  // Free all the SDL_Surfaces that were created
  SDL_FreeSurface(numberSurface);
  SDL_FreeSurface(scoreSurface);
  SDL_FreeSurface(screenSurface);
  SDL_FreeSurface(maskSurface);

  // Create the frame buffer
  glGenFramebuffers(1, &m_fboId);
  glBindFramebuffer(GL_FRAMEBUFFER, m_fboId);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fboGameOverTexId, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fboLifeTexId, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fboNumTexId, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fboMaskTexId, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fboTextTexId, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fboGameTexId, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_fboGameDepthId, 0);

  // Set the fragment shader output targets (DEPTH_ATTACHMENT is done automatically)
  GLenum drawBufs[] = {GL_COLOR_ATTACHMENT0};
  glDrawBuffers(1, drawBufs);

  // Unbind the framebuffer to revert to default render pipeline
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//void Game::initShaderProgram(const std::string &_name)
//{
//  // grab an instance of shader manager
//  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
//  // create a shader program with our given name
//  shader->createShaderProgram(_name);
//  // now we are going to create empty shaders for Frag and Vert
//  shader->attachShader(_name + "Vertex", ngl::ShaderType::VERTEX);
//  shader->attachShader(_name + "Fragment", ngl::ShaderType::FRAGMENT);
//  // attach the source
//  shader->loadShaderSource(_name + "Vertex", "shaders/" + _name + "Vertex.glsl");
//  shader->loadShaderSource(_name + "Fragment", "shaders/" + _name + "Fragment.glsl");
//  // compile the shaders
//  shader->compileShader(_name + "Vertex");
//  shader->compileShader(_name + "Fragment");
//  // add them to the program
//  shader->attachShaderToProgram(_name, _name + "Vertex");
//  shader->attachShaderToProgram(_name, _name + "Fragment");
//  // now we have associated this data we can link the shader
//  shader->linkProgramObject(_name);
//}

//void Game::initTexture(const GLuint& texUnit, GLuint &texId, const char *filename) {
//    // Set our active texture unit
//    glActiveTexture(GL_TEXTURE0 + texUnit);

//    // Load up the image using NGL routine
//    ngl::Image img(filename);

//    // Create storage for our new texture
//    glGenTextures(1, &texId);

//    // Bind the current texture
//    glBindTexture(GL_TEXTURE_2D, texId);

//    // Transfer image data onto the GPU using the teximage2D call
//    glTexImage2D (
//                GL_TEXTURE_2D,    // The target (in this case, which side of the cube)
//                0,                // Level of mipmap to load
//                img.format(),     // Internal format (number of colour components)
//                img.width(),      // Width in pixels
//                img.height(),     // Height in pixels
//                0,                // Border
//                GL_RGBA,          // Format of the pixel data
//                GL_UNSIGNED_BYTE, // Data type of pixel data
//                img.getPixels()); // Pointer to image data in memory

//    // Set up parameters for our texture
//    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//}
