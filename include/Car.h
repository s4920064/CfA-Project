#ifndef CAR_H_
#define CAR_H_

#include <ngl/Mat4.h>
#include <ngl/Vec3.h>
#include <ngl/Obj.h>
class Car
{
public :
  explicit Car(const ngl::Mat4 &_view, const ngl::Mat4 &_project, ngl::Vec3 _pos);
  void update();
  void draw();
  void forward();
  void backward();
  void stop();
  void left();
  void right();
  inline ngl::Vec3 getPos() const { return m_pos;}
  inline ngl::Vec3 getDir() const { return m_dir  ;}

private :
  void loadMatricesToShader() ;
  const ngl::Mat4 &m_view;
  const ngl::Mat4 &m_projection;
  float m_speed;
  ngl::Vec3 m_pos;
  ngl::Vec3 m_dir;
  float m_steerAngle;
  ngl::Obj *m_mesh;

};

#endif
