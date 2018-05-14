#include "Ship.h"
#include <ngl/ShaderLib.h>
#include <ngl/Vec3.h>
#include <ngl/Transformation.h>
#include <SDL_rect.h>

Ship::Ship( ngl::Vec3 _pos, float _lives, SDL_Rect &_moveBounds, ngl::Obj *_mesh )
{
  // set the position and step size
  m_position = _pos;
  // set the player bounds
  m_moveBounds = _moveBounds;
  // set the ship's mesh
  m_mesh = _mesh;
  // set the player lives
  m_lives = _lives;
  // set the ship state to true
  m_state = true;
}

Ship::~Ship()
{
}

void Ship::draw(ngl::Camera *_camera)
{
  // grab an instance of the shader manager
  ngl::ShaderLib *shader = ngl::ShaderLib::instance();
  // use the Ship shader for the following draws
  (*shader) ["Ship"]->use();

  // our MVP matrices
  ngl::Mat4 M;
  ngl::Mat4 MV;
  ngl::Mat4 MVP;
  ngl::Mat4 N;

  // to make transformation matrices
  ngl::Transformation t;

  // set the position of the ship
  t.setPosition(m_position);
  //t.setRotation(0,0,0);

  // set the MVP matrices
  M = t.getMatrix();
  MV = _camera->getViewMatrix() * M;
  MVP = _camera->getVPMatrix() * M;

  // send the MVP to the shader
  shader->setUniform("MVP", MVP);
  //shader->setUniform("N", N);
  //shader->setUniform("M", MVP);
  //shader->setUniform("VP", MVP);

  // send the state of the ship to the shader
  shader->setUniform("state", m_state);

  // draw the ship mesh
  m_mesh->draw();
}

void Ship::forward()
{
  // if the new position won't be past the top side of the bounds rect
  if(m_position.m_z - c_step > m_moveBounds.y)
  // then move it forward
  { m_position.m_z -= c_step; }
}

void Ship::backward()
{
  // if the new position won't be past the bottom side of the bounds rect
  if(m_position.m_z + c_step < m_moveBounds.y + m_moveBounds.h)
  // then move it backward
  { m_position.m_z += c_step; }
}

void Ship::left()
{
  // if the new position won't be past the left side of the bounds rect
  if(m_position.m_x - c_step > m_moveBounds.x)
  // then move it left
  { m_position.m_x -= c_step; }
}

void Ship::right()
{
  // if the new position won't be past the right side of the bounds rect
  if(m_position.m_x + c_step < m_moveBounds.x + m_moveBounds.w)
  // then move it right
  { m_position.m_x += c_step; }
}
