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
  // initialize the effect struct values
  m_effect.m_damaged = false;
  m_effect.m_display = false;
  m_effect.m_timer = 0;
  m_effect.m_color = ngl::Vec4(0,0,0,1);
}

Ship::~Ship()
{
}

void Ship::draw(ngl::Camera *_camera)
{
  // grab an instance of the shader manager
  ngl::ShaderLib *shader = ngl::ShaderLib::instance();
  // use the Ship shader for the following draws
  (*shader) ["Diffuse"]->use();

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

  // the color of the ship
  ngl::Vec4 color;

  // if there is an effect being displayed
  if(m_effect.m_display)
  {
    color = m_effect.m_color;
  }
  else
  {
    if(m_state)
    {
      color = ngl::Vec4(1,0,1,1);
    }
    else
    {
      color = ngl::Vec4(0,1,1,1);
    }
  }

  // send that color to the shader
  shader->setUniform("color", color);

  // draw the ship mesh
  m_mesh->draw();
}

void Ship::update()
{
  // if there is an effect going on
  if(m_effect.m_timer > 0)
  {
    if(m_effect.m_damaged)
    {
      m_effect.m_color = ngl::Vec4(1,0,0,1);
    }
    else
    {
      m_effect.m_color = ngl::Vec4(0,1,0,1);
    }
    // make the display flag flash on and off
    if((m_effect.m_timer/10) % 2 == 0)
    {
      m_effect.m_display = true;
    }
    else
    {
      m_effect.m_color = ngl::Vec4(1,1,1,1);
    }
    m_effect.m_timer--;
  }
  // if there is no effect going on
  else
  {
    // make sure no effect is displaying
    m_effect.m_display = false;
    m_effect.m_timer = 0;
  }
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

void Ship::setEffect(int _time, bool _damaged)
{
  m_effect.m_timer = _time;
  m_effect.m_damaged = _damaged;
}
