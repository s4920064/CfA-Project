#ifndef SHIP_H_
#define SHIP_H_
#include <ngl/Vec3.h>
#include <ngl/Obj.h>
#include <ngl/Camera.h>
#include <SDL_rect.h>

class Ship
{
public:
  Ship( ngl::Vec3 _pos, float _lives, SDL_Rect &_moveBounds, ngl::Obj *_mesh );
  ~Ship();

  void update();
  void draw(ngl::Camera *_camera);
  void forward();
  void backward();
  void left();
  void right();

  float m_lives;
  bool m_state;
  // the radius for the collision bounds sphere
  const float c_sphere = 1.0f;

private:
  // current position of the ship
  ngl::Vec3 m_position;
  // the step size for movement
  const float c_step = 0.1f;
  // a rect for the player movement area bounds
  SDL_Rect m_moveBounds;
  // a pointer for the mesh data of the ship
  ngl::Obj *m_mesh;
};

#endif
