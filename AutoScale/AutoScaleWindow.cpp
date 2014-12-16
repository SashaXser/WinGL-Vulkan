// local includes
#include "AutoScaleWindow.h"

// gl includes
#include <GL/glew.h>
#include <GL/GL.h>

// std incluces
#include <iostream>

AutoScaleWindow::AutoScaleWindow( )
{
}

AutoScaleWindow::~AutoScaleWindow( )
{
   
}

bool AutoScaleWindow::Create( unsigned int nWidth,
                              unsigned int nHeight,
                              const char * pWndTitle,
                              const void * pInitParams )
{
   // initialize with a context else nothing
   const OpenGLWindow::OpenGLInit glInit[] =
   {
      { 4, 1, false, true, false },
      { 0 }
   };

   // call base class to init
   if (OpenGLWindow::Create(nWidth, nHeight, pWndTitle, glInit))
   {
      // make the context current
      MakeCurrent();

      // enable the depth buffer and cull faces
      glEnable(GL_CULL_FACE);
      glEnable(GL_DEPTH_TEST);
      
      // force the projection matrix to get calculated and updated
      SendMessage(GetHWND(), WM_SIZE, 0, nHeight << 16 | nWidth);

      // indicate what actions can be taken
      std::cout << std::endl
                << "1 - Manipulates camera" << std::endl
                << "2 - Manipulates directional lighting" << std::endl
                << std::endl
                << "a - Moves camera to the left" << std::endl
                << "d - Moves camera to the right" << std::endl
                << "w - Moves camera to the up" << std::endl
                << "s - Moves camera to the down" << std::endl
                << std::endl
                << "Shift + t - Changes texture" << std::endl
                << std::endl
                << "f - Switch to flat shading" << std::endl
                << "n - Switch to normal shading" << std::endl
                << "p - Switch to parallax shading" << std::endl
                << "t - Switch to tessellation shading" << std::endl
                << std::endl
                << "Shift + s - Turn point light on / off" << std::endl
                << std::endl
                << "L-Button Down - Activate camera / directional light rotation" << std::endl
                << "Mouse X / Y Delta - Manipulate camera / directional light rotation" << std::endl;

      return true;
   }

   return false;
}

void AutoScaleWindow::OnDestroy( )
{
   // call the base class destroy
   OpenGLWindow::OnDestroy();
}
            
int AutoScaleWindow::Run( )
{
   // vars for the loop
   int appQuitVal = 0;
   bool bQuit = false;

   // basic message pump and render frame
   while (!bQuit)
   {
      // process all the messages
      if (!(bQuit = PeekAppMessages(appQuitVal)))
      {
         // clear the back buffer and the depth buffer
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

         // swap the front and back buffers
         SwapBuffers(GetHDC());
      }
   }

   return appQuitVal;
}

LRESULT AutoScaleWindow::MessageHandler( UINT uMsg,
                                         WPARAM wParam,
                                         LPARAM lParam )
{
   LRESULT result = 0;

   switch (uMsg)
   {
   case WM_SIZE:
      {
      // get the width and height
      const int32_t width = static_cast< int32_t >(lParam & 0x0000FFFF);
      const int32_t height = static_cast< int32_t >(lParam >> 16);

      // change the size of the viewport
      // this will be ignored until a valid gl context is created
      glViewport(0, 0,
                 static_cast< GLsizei >(width),
                 static_cast< GLsizei >(height));

      // update the projection matrix
      }

      break;

   default:
      // allow default processing to happen
      result = OpenGLWindow::MessageHandler(uMsg, wParam, lParam);
   }
   
   return result;
}

