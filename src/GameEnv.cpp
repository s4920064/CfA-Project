#include "GameEnv.h"
#include <SDL2/SDL.h>

Grid::Grid()
{
  m_movementArea.w = 10;
  m_movementArea.h = 10;
}

Grid::~Grid()
{}


//// implement this function so that you can choose steps for width and height
//void VAOPrimitives::createLineGrid( const std::string &_name, Real _width,  Real _depth, int _steps ) noexcept
//{
//  // a std::vector to store our verts, remember vector packs contiguously so we can use it
//  std::vector <vertData> data;
//  vertData vert;
//    // claculate the step size for each grid value
//  Real wstep=_width/static_cast<Real>(_steps);
//  // pre-calc the offset for speed
//  Real ws2=_width/2.0f;
//  // assign v as our value to change each vertex pair
//  Real v1=-ws2;

//    // claculate the step size for each grid value
//  Real dstep=_depth/static_cast<Real>(_steps);
//  // pre-calc the offset for speed
//  Real ds2=_depth/2.0f;
//  // assign v as our value to change each vertex pair
//  Real v2=-ds2;

//    for(int i=0; i<=_steps; ++i)
//    {
//      // vertex 1 x,y,z
//      vert.x=-ws2; // x
//      vert.z=v1; // y
//      vert.y=0.0; // z
//      data.push_back(vert);
//      // vertex 2 x,y,z
//      vert.x=ws2; // x
//      vert.z=v1; // y
//      data.push_back(vert);


//      // vertex 1 x,y,z
//      vert.x=v2; // x
//      vert.z=ds2; // y
//      data.push_back(vert);
//      // vertex 2 x,y,z
//      vert.x=v2; // x
//      vert.z=-ds2; // y
//      data.push_back(vert);



//      // now change our step value
//      v1+=wstep;
//      v2+=dstep;
//    }

//  createVAO(_name,data,GL_LINES);

//}
