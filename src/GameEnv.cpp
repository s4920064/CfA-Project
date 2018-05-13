#include "GameEnv.h"
#include <SDL2/SDL.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>

GameEnv::GameEnv( std::string _texture )
{
  // set the texture
  ngl::Texture t(_texture);
  m_texID=t.setTextureGL();
  // grab an instance of the VAO Primitives class and store it in prim
  ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();
  // create a sphere primitive called "background"
  prim->createSphere("background",300,20);
}

GameEnv::~GameEnv()
{

}

GameEnv::draw(ngl::Camera *_camera)
{
  // grab an instance of the shader manager
  ngl::ShaderLib *shader = ngl::ShaderLib::instance();

  // grab an instance of the VAO Primitives class
  ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();

  // use the GameEnv texture for the following draws
  (*shader)["GameEnv"]->use();

  // use t to make transformation matrices
  ngl::Transformation t;

  // our MVP matrix
  ngl::Mat4 MVP;

  // set the MVP
  MVP = _camera->getVPMatrix() * t.getMatrix();
  // send the MVP to the shader
  shader->setUniform("MVP",MVP);
  // bind the texture
  glBindTexture(GL_TEXTURE_2D,m_texID);
  // draw the background
  prim->draw("background");

  // create a line grid
  prim->createLineGrid("grid", 100, 100, 20);
  // draw the grid
  prim->draw("grid");

}
