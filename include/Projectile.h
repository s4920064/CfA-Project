#ifndef PROJECTILE_H_
#define PROJECTILE_H_
#include <ngl/Vec3.h>
#include <ngl/Obj.h>
#include <ngl/Camera.h>
#include <SDL_rect.h>
#include <list>

class Projectile
{
public:
  Projectile( ngl::Vec3 _startPos, float _life, ngl::Obj *_mesh );
  ~Projectile();

  void update();
  void draw( ngl::Camera *_camera );
  bool m_state;
  /// @returns m_active
  inline bool isActive() const {return m_active;}
  // returns m_threat
  inline bool isThreat() const {return m_threat;}
  // current position of the projectile
  ngl::Vec3 m_position;

  // the radius for the collision bounds sphere
  const float c_sphere = 0.3f;
  // the projectile's state
  bool m_state;

private:
  // the step size for movement
  float m_speed;
  // a pointer for the mesh data of the ship
  ngl::Obj *m_mesh;
  /// @brief flag to indicate if the rocket is still alive
  bool m_active;
  // a flag to indicate if the rocket can collide with the ship
  bool m_threat;
  /// @brief the current life of the rocket
  int m_life;
  /// @brief the max lifespan of the rocket (set in the ctor at present)
  int m_maxLife;
};

#endif
