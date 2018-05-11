#ifndef PROJECTILE_H_
#define PROJECTILE_H_
#include <ngl/Vec3.h>
#include <ngl/Obj.h>
#include <ngl/Camera.h>
#include <SDL_rect.h>

class Projectile
{
public:
  Projectile( ngl::Vec3 _startPos, float _life, ngl::Obj *_mesh );
  ~Projectile();

  void update();
  void draw( ngl::Camera *_camera );
  bool collision( float shipRadius );

  bool m_state;

private:
  // current position of the ship
  ngl::Vec3 m_position;
  // the step size for movement
  float m_speed;
  // a rect for the player movement area bounds
  SDL_Rect m_moveBounds;
  // the radius for the collision bounds sphere
  const float c_sphere = 1.0f;
  // a pointer for the mesh data of the ship
  ngl::Obj *m_mesh;
  /// @brief flag to indicate if the rocket is still alive
  bool m_active;
  /// @brief the current life of the rocket
  int m_life;
  /// @brief the max lifespan of the rocket (set in the ctor at present)
  int m_maxLife;
};

#endif
