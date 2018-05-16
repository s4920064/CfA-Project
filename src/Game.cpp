#include "Game.h"
#include "Ship.h"
#include "GameEnv.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ngl/ShaderLib.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/NGLInit.h>
#include <ngl/Material.h>
#include <ngl/NGLStream.h>
#include <SDL2/SDL_rect.h>
#include <algorithm>

#include <ft2build.h>
#include FT_FREETYPE_H

Game::Game(int _h)
{
  // set width and height
  m_width = _h*1.5;
  m_height = _h;

  // set the time and score to zero, and lives to full
  m_time = 0;
  m_score = 0;
  m_lives = 3.0;

  // set the clear color to dark grey
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

  // Enable 2D texturing
  glEnable(GL_TEXTURE_2D);

  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);

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

  ///------------------The Ship-------------------
  shader->loadShader("Ship",
                     "shaders/ShipVertex.glsl",
                     "shaders/ShipFragment.glsl");

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
  shader->loadShader("Projectile",
                     "shaders/ProjectileVertex.glsl",
                     "shaders/ProjectileFragment.glsl");

  // the mesh
  m_projectileMesh = new ngl::Obj("models/Missile.obj");
  m_projectileMesh->createVAO();

  // set the number of active projectiles to zero
  m_activeProjectiles = 0;
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

//  if (FT_Init_FreeType(&m_ft))
//      std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

//  if (FT_New_Face(m_ft, "font/game_over.ttf", 0, &m_face))
//      std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

//  FT_Set_Pixel_Sizes(m_face, 0, 48);

//  if(FT_Load_Char(m_face, 'X', FT_LOAD_RENDER)) {
//    fprintf(stderr, "Could not load character 'X'\n");
//  }

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

void Game::resize(int _h)
{
  // set the width and height to the new values,
  // keeping a constant w/h ratio
  m_width = int(float(_h)*1.5f);
  m_height = _h;

  glViewport(0,0,m_width,m_height);
  // now set the camera size values as the screen size has changed
  m_camera->setShape(45,(float)m_width/m_height,0.05,350);

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

  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Set up the viewport
  glViewport(0,0,m_width,m_height);

  /// ---Draw the Environment---
  m_gameEnv->draw(m_camera);

  /// ---Draw the Ship---
  m_ship->draw(m_camera);

  /// ---Draw the Projectiles---
  std::list <Projectile>::iterator i;
  std::list <Projectile>::iterator end=m_projectiles.end();

  for(i = m_projectiles.begin(); i != end; i++)
  {
    i->draw(m_camera);
    //std::cout<< "drawing a projectile!!!!\n";
  }

  /// ---Draw the Final Image (on screen-oriented plane)--

  // Unbind our FBO (set to default)
  glBindFramebuffer(GL_FRAMEBUFFER,0);

  // Set background colour
  glClearColor(0.4f, 0.4f, 0.4f, 1.0f);

  // Clear the screen
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_width,m_height);

  // BottleFront texture bind
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, m_fboGameTexId);

  // Depth texture bind
  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, m_fboGameDepthId);

  // grab an instance of the shader manager
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  // use the Game shader program for this draw
  (*shader)["Game"]->use();
  GLuint pid = shader->getProgramID("Game");

  // send the game texture and depth to the GPU
  glUniform1i(glGetUniformLocation(pid, "gameTex"), 3);
  glUniform1i(glGetUniformLocation(pid, "depthTex"), 4);
  glUniform2f(glGetUniformLocation(pid, "windowSize"), m_width, m_height);

  // set the MVP for the plane
  glm::mat4 MVP = glm::rotate(glm::mat4(1.0f), glm::pi<float>() * 0.5f, glm::vec3(1.0f,0.0f,0.0f));
  glUniformMatrix4fv(glGetUniformLocation(pid, "MVP"), 1, false, glm::value_ptr(MVP));

  // Grab and instance of the VAO primitives path
  ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();
  // draw the plane
  prim->draw("plane");

  (*shader)["Projectile"]->use();

//  float sx = 2.0 / m_width;
//  float sy = 2.0 / m_height;


//  render_text("The Quick Brown Fox Jumps Over The Lazy Dog",
//                 sx,   sy,  sx, sy);

  // bind the plane texture
  glBindTexture(GL_TEXTURE_2D, 0);

}

void Game::update()
{
  m_time++;

  m_score = m_time / 40;
  //std::cout<< m_score << "\n";

  ///---------------Spawn Projectiles-------------
  // every 80 cycles
  if(m_time % 80 == 0)
  {
    // generate a random floating point value within the horizontal movebounds area
    float randX = float(rand() % (m_moveBounds.w*10))/10 + m_moveBounds.x;
    // create a new projectile at that point with speed 0.5
    Projectile *p = new Projectile( ngl::Vec3(randX,0,-100), 0.5f, m_projectileMesh);
    // push it onto the list of projectiles
    m_projectiles.push_back(*p);
    // increment the number of active projectiles
    ++m_activeProjectiles;
  }
  ///---------------------------------------------

  ///-------Update Projectiles and Collision------
  // iterator for the list of projectiles
  std::list <Projectile>::iterator p;
  std::list <Projectile>::iterator end=m_projectiles.end();

  // for every active projectile i
  for(p = m_projectiles.begin(); p != end; p++)
  {
    // update the projectile
    p->update(m_moveBounds.y, m_ship->c_sphere);

    // if it is active
    if(p->isActive())
    {
      // if it is a threat
      if(p->isThreat())
      {
        // distance between the ship and the projectile in question
        float distance = sqrt( pow(p->m_position.m_x - m_ship->getPos().m_x, 2.0f) +
                               pow(p->m_position.m_z - m_ship->getPos().m_z, 2.0f) );
        // if there is a collision between the projectile and the ship
        if(distance < p->c_sphere + m_ship->c_sphere)
        {
          // if they are different states
          if(p->m_state != m_ship->m_state)
          {
            // then remove one full life
            m_lives -= 1.0;

            // start the ship damage timer
            m_ship->m_damageTimer = 40;
          }
          // if they are of the same state then add a bit of life
          else { m_lives += 0.1; }

          // erase the projectile from the list
          m_projectiles.erase(p++);
        }
      }
    }
    // if it is not active, erase it from the list
    else {m_projectiles.erase(p++);}
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

void Game::move(float _x, float _z)
{
//  m_ship->forward(_z);
//  m_ship->backward(_z);
//  m_ship->left(_x);
//  m_ship->right(_x);
  // if the new position won't be past the top and bottom side of the bounds rect
  float shipZ = m_ship->getPos().m_z;
  if(shipZ + _z > m_moveBounds.y && shipZ + _z < m_moveBounds.y + m_moveBounds.h )
  { m_ship->setZ(shipZ+_z); }

  // if the new position won't be past the left and right side of the bounds rect
  float shipX = m_ship->getPos().m_x;
  if(shipX + _x > m_moveBounds.x && shipX + _x < m_moveBounds.x + m_moveBounds.w )
  { m_ship->setX(shipX+_x); }

}
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

  // The BottleFront pass is rendered to a texture buffer
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

  // The depth buffer is rendered to a texture buffer too
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

  // Create the frame buffer
  glGenFramebuffers(1, &m_fboId);
  glBindFramebuffer(GL_FRAMEBUFFER, m_fboId);

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

void Game::initTexture(const GLuint& texUnit, GLuint &texId, const char *filename) {
    // Set our active texture unit
    glActiveTexture(GL_TEXTURE0 + texUnit);

    // Load up the image using NGL routine
    ngl::Image img(filename);

    // Create storage for our new texture
    glGenTextures(1, &texId);

    // Bind the current texture
    glBindTexture(GL_TEXTURE_2D, texId);

    // Transfer image data onto the GPU using the teximage2D call
    glTexImage2D (
                GL_TEXTURE_2D,    // The target (in this case, which side of the cube)
                0,                // Level of mipmap to load
                img.format(),     // Internal format (number of colour components)
                img.width(),      // Width in pixels
                img.height(),     // Height in pixels
                0,                // Border
                GL_RGBA,          // Format of the pixel data
                GL_UNSIGNED_BYTE, // Data type of pixel data
                img.getPixels()); // Pointer to image data in memory

    // Set up parameters for our texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void Game::render_text(const char *text, float x, float y, float sx, float sy) {
  const char *p;

  for(p = text; *p; p++) {
//    if(FT_Load_Char(m_face, *p, FT_LOAD_RENDER))
//        continue;

    glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_RED,
      m_g->bitmap.width,
      m_g->bitmap.rows,
      0,
      GL_RED,
      GL_UNSIGNED_BYTE,
      m_g->bitmap.buffer
    );

    float x2 = x + m_g->bitmap_left * sx;
    float y2 = -y - m_g->bitmap_top * sy;
    float w = m_g->bitmap.width * sx;
    float h = m_g->bitmap.rows * sy;

    GLfloat box[4][4] = {
        {x2,     -y2    , 0, 0},
        {x2 + w, -y2    , 1, 0},
        {x2,     -y2 - h, 0, 1},
        {x2 + w, -y2 - h, 1, 1},
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    x += (m_g->advance.x/64) * sx;
    y += (m_g->advance.y/64) * sy;
  }
}
