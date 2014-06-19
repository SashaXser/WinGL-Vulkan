// local includes
#include "TessellationTriangleWindow.h"
//#include "Matrix.h"
//#include "Vector3.h"
//#include "WglAssert.h"
//#include "MatrixHelper.h"
//#include "OpenGLExtensions.h"

// std includes
//#include <vector>
//#include <cstdlib>
//#include <cstdint>
//#include <iostream>

TessellationTriangleWindow::TessellationTriangleWindow( )
{
}

TessellationTriangleWindow::~TessellationTriangleWindow( )
{
}

bool TessellationTriangleWindow::Create( unsigned int nWidth,
                                         unsigned int nHeight,
                                         const char * pWndTitle,
                                         const void * pInitParams )
{
   // indicate the contexts to initialize with
   const OpenGLWindow::OpenGLInit glInit[] =
   {
      { 4, 4, false, true, false },
      { 4, 4, false, false, false },
      { 4, 3, false, true, false },
      { 4, 3, false, false, false },
      { 0 }
   };

   // call base class to init
   if (OpenGLWindow::Create(nWidth, nHeight, pWndTitle, glInit))
   {
      // make the context current
      MakeCurrent();
  
      return true;
   }
   else
   {
      // post the quit message
      PostQuitMessage(-1);
   }

   return false;
}

int TessellationTriangleWindow::Run( )
{
   // app quit variables
   int appQuitVal = 0;
   bool bQuit = false;

   while (!bQuit)
   {
      // process all the app messages and then render the scene
      if (!(bQuit = PeekAppMessages(appQuitVal)))
      {
      }
   }

   return appQuitVal;
}

LRESULT TessellationTriangleWindow::MessageHandler( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
   LRESULT result = 0;

   switch (uMsg)
   {
   default:
      // default handle the messages
      result = OpenGLWindow::MessageHandler(uMsg, wParam, lParam);
   }

   return result;
}

