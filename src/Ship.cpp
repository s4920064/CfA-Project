#include "Ship.h"

Ship::Ship()
{
  m_pos=_pos;
  m_dir.set(0,0,-1);
  m_pos.m_y=0.5;
  m_mesh= new ngl::Obj("models/buggy.obj");
  m_mesh->createVAO();
}
