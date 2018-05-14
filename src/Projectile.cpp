#include "Projectile.h"
#include <ngl/Vec3.h>
#include <ngl/Obj.h>
#include <ngl/Camera.h>
#include <ngl/ShaderLib.h>
#include <ngl/Transformation.h>
#include <SDL_rect.h>
#include <random>

Projectile::Projectile( ngl::Vec3 _startPos, float _speed, ngl::Obj *_mesh )
{
  m_position = _startPos;
  m_life = 0;
  m_speed = _speed;
  m_active = true;
  m_threat = false;
  m_state = rand() % 2;
  m_maxLife = 3000;
  m_mesh = _mesh;
}

Projectile::~Projectile()
{

}

// update the projectile's position and flags
void Projectile::update( float _moveBoundsY )
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
    if(m_position.m_z > _moveBoundsY)
    {
      // then flag it as a threat
      m_threat = true;
    }
  }
}

// draw the projectile
void Projectile::draw( ngl::Camera *_camera )
{
  // grab an instance of the shader manager
  ngl::ShaderLib *shader = ngl::ShaderLib::instance();
  // use the Ship shader for the following draws
  (*shader) ["Projectile"]->use();

  // our MVP matrices
  ngl::Mat4 M;
  ngl::Mat4 MV;
  ngl::Mat4 MVP;
  ngl::Mat4 N;

  // to make transformation matrices
  ngl::Transformation t;

  // set the position of the projectile
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

  // draw the projectile mesh
  m_mesh->draw();
}
