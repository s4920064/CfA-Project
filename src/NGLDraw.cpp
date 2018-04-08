#include "NGLDraw.h"
#include <ngl/ShaderLib.h>
#include <ngl/NGLInit.h>
#include <ngl/Material.h>
#include <ngl/NGLStream.h>


NGLDraw::NGLDraw()
{
  glClearColor(0.4f, 0.4f, 0.4f, 1.0f);			   // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
   // now to load the shader and set the values
  // grab an instance of shader manager
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  (*shader)["nglDiffuseShader"]->use();

  shader->setUniform("Colour",1.0f,1.0f,1.0f,1.0f);
  shader->setUniform("lightPos",1.0f,1.0f,1.0f);
  shader->setUniform("lightDiffuse",1.0f,1.0f,1.0f,1.0f);

  ngl::Vec3 from(0,5,20);
  ngl::Vec3 to(0,0,0);
  ngl::Vec3 up(0,1,0);
  m_view=ngl::lookAt(from,to,up);
  ngl::VAOPrimitives::instance()->createLineGrid("grid",1400,1400,250);
  m_car = new Car(m_view,m_projection,ngl::Vec3(0.0f,0.0f,0.0f));
}

NGLDraw::~NGLDraw()
{
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
}

void NGLDraw::resize(int _w, int _h)
{
  glViewport(0,0,_w,_h);
  m_projection=ngl::perspective(45,(float)_w/_h,0.05,350);

}

void NGLDraw::draw()
{
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // now to load the shader and set the values
  // grab an instance of shader manager
  ngl::Vec3 pos=m_car->getPos();
  ngl::Vec3 dir=m_car->getDir();
  pos+=ngl::Vec3(0,1,5);
  std::cout<<pos<<"\n";
  //pos+=dir;
  m_view=ngl::lookAt(pos,m_car->getPos(),ngl::Vec3(0,1,0));

  loadMatricesToShader();
  ngl::VAOPrimitives::instance()->draw("grid");
  m_car->draw();
}


void NGLDraw::loadMatricesToShader()
{
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  shader->use(ngl::nglDiffuseShader);
  ngl::Mat4 MV;
  ngl::Mat4 MVP;
  ngl::Mat3 normalMatrix;
  MVP= m_projection*m_view;
  normalMatrix=MV;
  normalMatrix.inverse();
  shader->setUniform("Colour",1,1,1,1);
  shader->setUniform("MVP",MVP);
  shader->setUniform("normalMatrix",normalMatrix);
}

//----------------------------------------------------------------------------------------------------------------------
void NGLDraw::update()
{
  m_car->update();
}

//----------------------------------------------------------------------------------------------------------------------
void NGLDraw::keyEvent(SDL_KeyboardEvent &_event)
{
  switch (_event.keysym.sym)
  {
    case SDLK_UP :
      m_car->forward();
    break;
    case SDLK_DOWN :
      m_car->backward();
    break;
    case SDLK_LEFT :
      m_car->left();
    break;
    case SDLK_RIGHT :
      m_car->right();
    break;

    case SDLK_SPACE :
      m_car->stop();
    break;
    default:
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------



//----------------------------------------------------------------------------------------------------------------------
void NGLDraw::mouseMoveEvent (const SDL_MouseMotionEvent &_event)
{
}


//----------------------------------------------------------------------------------------------------------------------
void NGLDraw::mousePressEvent (const SDL_MouseButtonEvent &_event)
{
}

//----------------------------------------------------------------------------------------------------------------------
void NGLDraw::mouseReleaseEvent (const SDL_MouseButtonEvent &_event)
{
}

//----------------------------------------------------------------------------------------------------------------------
void NGLDraw::wheelEvent(const SDL_MouseWheelEvent &_event)
{
}
//----------------------------------------------------------------------------------------------------------------------
