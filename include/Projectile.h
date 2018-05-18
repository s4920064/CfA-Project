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
  Projectile( ngl::Vec3 _startPos, float _speed, ngl::Obj *_mesh );
  ~Projectile();

  virtual void update(float _moveBoundsY, float _shipRadius);
  virtual void draw( ngl::Camera *_camera );
  // the projectile's state, this will determine the color
  bool m_state;
  /// @returns m_active
  inline bool isActive() const {return m_active;}
  // returns m_threat
  inline bool isThreat() const {return m_threat;}
  // current position of the projectile
  ngl::Vec3 m_position;
  // the radius for the collision bounds sphere
  const float c_sphere = 0.1f;
  // returns the string with the type
  inline const char* getType() {return c_type;}

protected:
  // the step size for movement
  float m_speed;
  // the step size for the rotation
  float m_rotSpeed;
  // a pointer for the mesh data of the ship
  ngl::Obj *m_mesh;
  // current rotation of the projectile
  ngl::Vec3 m_rotation;
  /// @brief flag to indicate if the rocket is still alive
  bool m_active;
  // a flag to indicate if the rocket can collide with the ship
  bool m_threat;
  /// @brief the current life of the rocket
  int m_life;
  /// @brief the max lifespan of the rocket (set in the ctor at present)
  int m_maxLife;
  // string specifiying the class type
  const char *c_type;
};

class Missile : public Projectile
{
public :
  Missile( ngl::Vec3 _startPos, float _speed, ngl::Obj *_mesh );
  ~Missile();
  void update(float _moveBoundsY, float _shipRadius);
  void draw(ngl::Camera *_camera);
};

class Obstacle : public Projectile
{
public :
  Obstacle( ngl::Vec3 _startPos, float _speed, ngl::Obj *_mesh );
  ~Obstacle();
  void update(float _moveBoundsY, float _shipRadius);
  void draw(ngl::Camera *_camera);
};

class Bonus : public Projectile
{
public :
  Bonus( ngl::Vec3 _startPos, float _speed, ngl::Obj *_mesh );
  ~Bonus();
  void update(float _moveBoundsY, float _shipRadius);
  void draw(ngl::Camera *_camera);
};

#endif
