#include "GameEnv.h"
#include <ngl/Texture.h>
#include <ngl/Camera.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>
#include <ngl/Transformation.h>

GameEnv::GameEnv( /*std::string _texture*/ )
{
  // set the texture
  //ngl::Texture t(_texture);
  //m_textureID=t.setTextureGL();
  // grab an instance of the VAO Primitives class and store it in prim
  ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();
  // create a sphere primitive called "background"
  prim->createSphere("background",300,20);
}

GameEnv::~GameEnv()
{

}

void GameEnv::draw(ngl::Camera *_camera)
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

  bool isGrid = true;

  // set the MVP
  MVP = _camera->getVPMatrix() * t.getMatrix();
  // send the MVP to the shader
  shader->setUniform("MVP",MVP);

  // bind the texture
  //glBindTexture(GL_TEXTURE_2D,m_textureID);

  shader->setUniform("isGrid",isGrid);
  // create a line grid
  prim->createLineGrid("grid", 200, 200, 100);
  // draw the grid
  prim->draw("grid");

  isGrid = false;

  shader->setUniform("isGrid",isGrid);
  // draw the background
  prim->draw("background");

}
