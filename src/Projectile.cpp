#include "Projectile.h"
#include <ngl/Vec3.h>
#include <ngl/Obj.h>
#include <ngl/Camera.h>
#include <SDL_rect.h>

Projectile::Projectile( ngl::Vec3 _startPos, float _speed, ngl::Obj *_mesh )
{
  m_position = _startPos;
  m_life = 0;
  m_speed = _speed;

}

Projectile::~Projectile()
{

}

void Projectile::update()
{

}

void Projectile::draw( ngl::Camera *_camera )
{

}

bool collision( float shipRadius )
{

}
