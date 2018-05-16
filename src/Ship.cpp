#include "Ship.h"
#include <iostream>
#include <ngl/ShaderLib.h>
#include <ngl/Vec3.h>
#include <ngl/Transformation.h>
#include <SDL_rect.h>

Ship::Ship( ngl::Vec3 _pos, SDL_Rect &_moveBounds, ngl::Obj *_mesh )
{
  // set the position and step size
  m_position = _pos;
  // set the player bounds
  m_moveBounds = _moveBounds;
  // set the ship's mesh
  m_mesh = _mesh;
  // set the ship state to true
  m_state = true;
  // set the ship damage state to false
  m_damaged = false;

  m_damageTimer = 0;
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
  //std::cout<<m_position.m_x<<" , "<<m_position.m_z<<"\n";
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
  shader->setUniform("damaged", m_damaged);

  // draw the ship mesh
  m_mesh->draw();
}

void Ship::update()
{
  if(m_damageTimer > 0)
  {
    if((m_damageTimer/10) % 2 == 0)
    {
      m_damaged = true;
    }
    else {m_damaged = false;}
    m_damageTimer--;
  }
  else { m_damaged = false; }
}

void Ship::forward(int _z)
{
  // if the new position won't be past the top side of the bounds rect
  if(m_position.m_z - c_step*_z > m_moveBounds.y)
  // then move it forward
  { m_position.m_z -= c_step*_z; }
}

void Ship::backward(int _z)
{
  // if the new position won't be past the bottom side of the bounds rect
  if(m_position.m_z + c_step*_z < m_moveBounds.y + m_moveBounds.h)
  // then move it backward
  { m_position.m_z = m_position.m_z+c_step*_z; std::cout<<"moving it backwards!!!!!!!!!!!!!!\n";  }
  std::cout<<m_position.m_z<<"\n";
  std::cout<<m_position.m_z+c_step*_z<<"\n";
  //std::cout<<m_moveBounds.y + m_moveBounds.h<<"\n";
}

void Ship::left(int _x)
{
  // if the new position won't be past the left side of the bounds rect
  if(m_position.m_x - c_step*_x > m_moveBounds.x)
  // then move it left
  { m_position.m_x -= c_step*_x; }
}

void Ship::right(int _x)
{
  // if the new position won't be past the right side of the bounds rect
  if(m_position.m_x + c_step*_x < m_moveBounds.x + m_moveBounds.w)
  // then move it right
  { m_position.m_x += c_step*_x; }
}
