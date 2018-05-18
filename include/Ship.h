#ifndef SHIP_H_
#define SHIP_H_
#include <ngl/Vec3.h>
#include <ngl/Obj.h>
#include <ngl/Camera.h>
#include <SDL_rect.h>

// a struct to hold information about a currently active effect
struct Effect
{
  // a timer to display the colored effect (eg. red damage blink)
  int m_timer;
  bool m_display;
  bool m_damaged;
  ngl::Vec4 m_color;
};

// a class for our ship
class Ship
{
public:
  Ship( ngl::Vec3 _pos, SDL_Rect &_moveBounds, ngl::Obj *_mesh );
  ~Ship();

  void draw(ngl::Camera *_camera);
  void update();
  void forward(int _z);
  void backward(int _z);
  void left(int _x);
  void right(int _x);
  inline void changeState() {m_state = !m_state;}
  inline ngl::Vec3 getPos() const {return m_position;}
  inline void setX(float _x) { m_position.m_x = _x; }
  inline void setZ(float _z) { m_position.m_z = _z; }
  void setEffect(int _time, bool _damaged);
  bool m_state;

  // the radius for the collision bounds sphere
  const float c_sphere = 0.5f;

private:
  // current position of the ship
  ngl::Vec3     m_position;
  // the step size for movement
  const float   c_step = 0.5f;
  // holds info about any currently active effect
  Effect m_effect;
  // a rect for the player movement area bounds
  SDL_Rect m_moveBounds;
  // a pointer for the mesh data of the ship
  ngl::Obj *m_mesh;
};

#endif
