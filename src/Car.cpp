#include "Car.h"
#include <ngl/ShaderLib.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/NGLStream.h>
#include <ngl/Transformation.h>

const static float UPDATEANGLE=4.0;
const static float UPDATESPEED=0.2;


Car::Car(const ngl::Mat4 &_view, const ngl::Mat4 &_project, ngl::Vec3 _pos) : m_view(_view),m_projection(_project)
{
  m_pos=_pos;
  m_dir.set(0,0,-1);
  m_speed=0.0f;
  m_steerAngle=0.0f;
  m_pos.m_y=0.5;
  m_mesh= new ngl::Obj("models/buggy.obj");
  m_mesh->createVAO();

}


void Car::draw()
{

  loadMatricesToShader();

 // ngl::VAOPrimitives::instance()->draw("cube");
  m_mesh->draw();
}

void Car::loadMatricesToShader()
{
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  shader->setUniform("Colour",1.0f,0.0f,0.0f,0.0f);

  ngl::Mat4 MV;
  ngl::Mat4 MVP;
  ngl::Mat3 normalMatrix;
  ngl::Mat4 M;

  ngl::Transformation t;
  t.setPosition(m_pos);
  //t.setScale(1.0,1.0,2.5);
  t.setRotation(0,m_steerAngle+180,0);
  std::cout<<"steer "<<m_steerAngle<<"\n";
  MVP= (m_projection) * (m_view) * t.getMatrix();
  normalMatrix=MV;
  normalMatrix.inverse();
  shader->setUniform("MVP",MVP);
  shader->setUniform("normalMatrix",normalMatrix);
}

void Car::update()
{
  std::cout<<"update "<<m_speed<<"\n";
  ngl::Mat3 rot;

  rot.rotateY(m_steerAngle);
  ngl::Vec3 dir(0.0,0.0,-1.0);
  m_dir=dir*rot;
  m_dir*=m_speed;
  std::cout<<m_dir<<"\n";
  m_pos+=m_dir;
  std::cerr<<"dir "<<m_dir<<" pos "<<m_pos<<" speed "<<m_speed<<" r "<<m_steerAngle<<"\n";

}

void Car::stop()
{
  m_speed=0.0f;
  //m_pos.set(0,0,0);
  //m_dir.set(0,0,0);
  //m_steerAngle=0.0;
  m_pos.m_y=0.5;

}

double clamp(double d, double min, double max)
{
  const double t = d < min ? min : d;
  return t > max ? max : t;
}


void Car::forward()
{
  m_speed+=UPDATESPEED;
  m_speed=clamp(m_speed,0.0,2.0);
}

void Car::backward()
{
  m_speed-=UPDATESPEED;
  m_speed=clamp(m_speed,-2.0,0.0);

}


void Car::left()
{
  m_steerAngle+=UPDATEANGLE;
  m_steerAngle=clamp(m_steerAngle,-180.0,180.0);
}

void Car::right()
{
  m_steerAngle-=UPDATEANGLE;
  m_steerAngle=clamp(m_steerAngle,-180.0,180.0);

}
