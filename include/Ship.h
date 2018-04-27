#ifndef SHIP_H__
#define SHIP_H__
#include <ngl/Vec3.h>
#include <ngl/Obj.h>

class Ship
{
public:
  Ship();
  ~Ship();

  void update();
  void draw();
  void forward();
  void backward();
  void left();
  void right();

  int m_lives;
  bool m_state;

private:
  ngl::Vec3 m_position;
  ngl::Vec3 m_direction;
  ngl::Obj *m_mesh;

  bool outofBounds();
  bool collision();
};

#endif
