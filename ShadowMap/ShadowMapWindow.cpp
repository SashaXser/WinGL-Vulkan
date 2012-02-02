// local includes
#include "ShadowMapWindow.h"
#include "WglAssert.h"
#include "OpenGLExtensions.h"

// gl includes
#include <gl/GL.h>

ShadowMapWindow::ShadowMapWindow( )
{
}

ShadowMapWindow::~ShadowMapWindow( )
{
}

bool ShadowMapWindow::Create( unsigned int nWidth,
                              unsigned int nHeight,
                              const char * pWndTitle,
                              const void * pInitParams )
{
   // initialize with a 4.0 context
   const OpenGLWindow::OpenGLInit glInit40 =
   {
      4, 0, true, true, false
   };

   // initialize with a 3.2 context
   const OpenGLWindow::OpenGLInit glInit32 =
   {
      3, 3, true, false, false
   };

   // initialize 40 first, then 32 second, else nothing
   const OpenGLWindow::OpenGLInit * glInit[] =
   {
      &glInit40, &glInit32, NULL
   };

   // call base class to init
   if (OpenGLWindow::Create(nWidth, nHeight, pWndTitle,
                            reinterpret_cast< const void ** >(glInit)))
   {
      // make the context current
      MakeCurrent();

      // attach to the debug context
      AttachToDebugContext();
      
      return true;
   }
   else
   {
      // post the quit message
      PostQuitMessage(-1);
   }

   return false;
}

int ShadowMapWindow::Run( )
{
   // app quit variables
   int appQuitVal = 0;
   bool bQuit = false;

   while (!bQuit)
   {
      // process all the app messages and then render the scene
      if (!(bQuit = PeekAppMessages(appQuitVal)))
      {
         //RenderScene();
      }
   }

   return appQuitVal;
}

LRESULT ShadowMapWindow::MessageHandler( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
   //switch (uMsg)
   //{
   //default:
      // default handle the messages
      return OpenGLWindow::MessageHandler(uMsg, wParam, lParam);
   //}
}
