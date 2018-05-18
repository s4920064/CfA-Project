#include "Projectile.h"
#include <ngl/Vec3.h>
#include <ngl/Obj.h>
#include <ngl/Camera.h>
#include <ngl/ShaderLib.h>
#include <ngl/Transformation.h>
#include <SDL_rect.h>
#include <random>

///---------------------------------BASE: Projectile---------------------------------


Projectile::Projectile( ngl::Vec3 _startPos, float _speed, ngl::Obj *_mesh )
{
  m_position = _startPos;
  m_life = 0;
  m_speed = _speed;
  m_rotSpeed = 5;
  m_active = true;
  m_threat = false;
  m_maxLife = 250;
  m_mesh = _mesh;
}

Projectile::~Projectile()
{

}

// update the projectile's position and flags
void Projectile::update( float _moveBoundsY, float _shipRadius)
{
  // update the projectile's position based on the speed
  m_position.m_z += m_speed;

  // if the projectile has lived for as long as it should
  if(++m_life >= m_maxLife)
  {
    // then flag it as no longer active
    m_active = false;
  }
  else
  {
    // if the projectile is in range to collide with the ship
    if(m_position.m_z > _moveBoundsY-_shipRadius)
    {
      // then flag it as a threat
      m_threat = true;
    }
  }
}

// a base method that sets the MVP to the GPU
void Projectile::draw( ngl::Camera *_camera )
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

  // set the position of the projectile
  t.setPosition(m_position);
  t.setRotation(m_rotation);

  // set the MVP matrices
  M = t.getMatrix();
  MV = _camera->getViewMatrix() * M;
  MVP = _camera->getVPMatrix() * M;

  // send the MVP to the shader
  shader->setUniform("MVP", MVP);
  //shader->setUniform("N", N);
  //shader->setUniform("M", MVP);
  //shader->setUniform("VP", MVP);
}

///---------------------------------CHILD: Missile---------------------------------

// Missile constructor with a call to the base class constructor by
// including it in the initialization list. Adds a few things special to this class.
Missile::Missile( ngl::Vec3 _startPos, float _speed, ngl::Obj *_mesh )
  : Projectile(_startPos, _speed, _mesh)
{
  // set the rotation to start at a random point around the z axis
  m_rotation.m_z = rand() % 360;
  // set the state to signify either purple or turquoise
  m_state = rand() % 2;
  // set the type specifier
  c_type = "missile";
}

// Missile update function that overrides the base class update method.
// makes a call to the base class update method and also adds things.
void Missile::update(float _moveBoundsY, float _shipRadius)
{
  // use all the same logic the base class does
  Projectile::update(_moveBoundsY, _shipRadius);

  // but also update the projectile's rotation
  // around the z axis based on the rotation speed
  m_rotation.m_z += m_rotSpeed;
}

void Missile::draw(ngl::Camera *_camera)
{
  // call the base class method draw to set the MVP
  Projectile::draw(_camera);

  // grab an instance of the shader manager
  ngl::ShaderLib *shader = ngl::ShaderLib::instance();

  // use the Ship shader for the following draws
  (*shader) ["Diffuse"]->use();

  // the color that will be used in our shader
  ngl::Vec4 color;

  // determine the color of the missile according to its state
  if(m_state)
  {
    // purple
    color = ngl::Vec4(1,0,1,1);
  }
  else
  {
    //turquoise
    color = ngl::Vec4(0,1,1,1);
  }

  // send that color to the shader
  shader->setUniform("color", color);

  // draw the projectile mesh
  m_mesh->draw();
}

///---------------------------------CHILD: Obstacle---------------------------------

// Obstacle constructor with a call to the base class constructor by
// including it in the initialization list. Adds a few things special to this class.
Obstacle::Obstacle( ngl::Vec3 _startPos, float _speed, ngl::Obj *_mesh )
  : Projectile(_startPos, _speed, _mesh)
{
  m_rotation = rand() % 360; //check if this needs to be redone for all axes
  // set the obstacle's state to signify red
  m_state = 3;
  // set the type specifier
  c_type = "obstacle";
}

// Obstacle update function that overrides the base class update method.
// makes a call to the base class update method and also adds things.
void Obstacle::update(float _moveBoundsY, float _shipRadius)
{
  // use all the same logic the base class does
  Projectile::update(_moveBoundsY, _shipRadius);

  // but also update the projectile's rotation
  // around all axes based on the rotation speed
  m_rotation.m_x += m_rotSpeed;
  m_rotation.m_y += m_rotSpeed;
  m_rotation.m_z += m_rotSpeed;
}

void Obstacle::draw(ngl::Camera *_camera)
{
  // call the base class method draw to set the MVP
  Projectile::draw(_camera);

  // grab an instance of the shader manager
  ngl::ShaderLib *shader = ngl::ShaderLib::instance();

  // use the Ship shader for the following draws
  (*shader) ["Diffuse"]->use();

  // the color that will be used in our shader
  ngl::Vec4 color = ngl::Vec4(1,0,0,1);

  // send that color to the shader
  shader->setUniform("color", color);

  // draw the projectile mesh
  m_mesh->draw();
}

///---------------------------------CHILD: Bonus---------------------------------

// Bonus constructor with a call to the base class constructor by
// including it in the initialization list. Adds a few things special to this class.
Bonus::Bonus( ngl::Vec3 _startPos, float _speed, ngl::Obj *_mesh )
  : Projectile(_startPos, _speed, _mesh)
{
  m_rotation = rand() % 360; //check if this needs to be redone for all axes
  // set the state to signify either purple or turquoise
  m_state = rand() % 2;
  // set the type specifier
  c_type = "bonus";
}

// Missile update function that overrides the base class update method.
// makes a call to the base class update method and also adds things.
void Bonus::update(float _moveBoundsY, float _shipRadius)
{
  // use all the same logic the base class does
  Projectile::update(_moveBoundsY, _shipRadius);

  // but also update the projectile's rotation
  // around all axes based on the rotation speed
  m_rotation.m_x += m_rotSpeed;
  m_rotation.m_y += m_rotSpeed;
  m_rotation.m_z += m_rotSpeed;
}

void Bonus::draw(ngl::Camera *_camera)
{
  // call the base class method draw to set the MVP
  Projectile::draw(_camera);

  // grab an instance of the shader manager
  ngl::ShaderLib *shader = ngl::ShaderLib::instance();

  // use the Ship shader for the following draws
  (*shader) ["Diffuse"]->use();

  // the color that will be used in our shader
  ngl::Vec4 color;

  // determine the color of the missile according to its state
  if(m_state)
  {
    // purple
    color = ngl::Vec4(1,0,1,1);
  }
  else
  {
    //turquoise
    color = ngl::Vec4(0,1,1,1);
  }

  // send that color to the shader
  shader->setUniform("color", color);

  // draw the projectile mesh
  m_mesh->draw();
}
