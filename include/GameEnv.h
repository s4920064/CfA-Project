#ifndef GAMEENV_H__
#define GAMEENV_H__
#include <ngl/Camera.h>
#include <string>

class GameEnv
{
public:
  GameEnv( /*std::string _texture*/ );
  ~GameEnv();

  // a method that draws the environment for the game
  void draw( ngl::Camera *_camera );

  void update(float _speed);

private:
  // the id where we store our texture
  //GLuint m_textureID;

  // the model matrix
  ngl::Mat4 m_model;

  float m_gridPos;

};

#endif
