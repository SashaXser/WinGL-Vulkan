// local includes
#include "OpenGLWindow.h"
#include "OpenGLExtensions.h"

// opengl includes
#include <gl/gl.h>

OpenGLWindow::OpenGLWindow( ) :
mGLContext     ( NULL )
{
}

OpenGLWindow::~OpenGLWindow( )
{
   ReleaseOpenGLContext();
}

bool OpenGLWindow::Create( unsigned int nWidth,
                           unsigned int nHeight,
                           const char * pWndTitle,
                           const void ** pInitParams )
{
   // create the main window
   if (Window::Create(nWidth, nHeight, pWndTitle, NULL))
   {
      // create the opengl context
      return CreateOpenGLContext(reinterpret_cast< const OpenGLInit ** >(pInitParams));
   }

   return false;
}

bool OpenGLWindow::CreateOpenGLContext( const OpenGLInit ** pInitParams )
{
   // create a pixel format descriptor
   PIXELFORMATDESCRIPTOR pfd;

   // clear out the pfd
   memset(&pfd, 0x00, sizeof(pfd));

   // setup basic attributes
   pfd.nSize = sizeof(pfd);
   pfd.nVersion = 1;
   pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
   pfd.iPixelType = PFD_TYPE_RGBA;
   pfd.cColorBits = 24;
   pfd.cAlphaBits = 8;
   pfd.cDepthBits = 32;
   pfd.iLayerType = PFD_MAIN_PLANE;

   // obtain the pixel format
   const HDC hDC = GetHDC();
   const int pixelFormat = ChoosePixelFormat(hDC, &pfd);

   // set the pixel format
   SetPixelFormat(hDC, pixelFormat, &pfd);

   // create the basic opengl context
   const HGLRC basicContext = wglCreateContext(hDC);

   // make basic current
   wglMakeCurrent(hDC, basicContext);

   // initialize the opengl extensions
   // need to reinterpret the function sig to make x64 happy...
   OpenGLExt::InitializeOpenGLExtensions(
      reinterpret_cast< int (* (__stdcall *)( const char * ))( void ) >
      (&wglGetProcAddress));

   // release the basic context
   wglMakeCurrent(hDC, NULL);

   if (pInitParams)
   {
      // destroy the basic context
      wglDeleteContext(basicContext);

      for (; !mGLContext && *pInitParams; ++pInitParams)
      {
         // create a new context
         int attribList[] =
         {
            OpenGLExt::WGL_CONTEXT_MAJOR_VERSION_ARB, 0,
            OpenGLExt::WGL_CONTEXT_MINOR_VERSION_ARB, 0,
            OpenGLExt::WGL_CONTEXT_PROFILE_MASK_ARB, 0,
            OpenGLExt::WGL_CONTEXT_FLAGS_ARB, 0,
            0, 0
         };
         
         // create the specified context...
         attribList[1] = (*pInitParams)->nMajorVer;
         attribList[3] = (*pInitParams)->nMinorVer;
         attribList[5] = (*pInitParams)->bCompatibleContext ?
                         OpenGLExt::WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB :
                         OpenGLExt::WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
         attribList[7] = (*pInitParams)->bEnableDebug ?
                         OpenGLExt::WGL_CONTEXT_DEBUG_BIT_ARB : 0;

         // store a 2.1 value and the requested
         // context in two byte of data
         const short CONTEXT_TWO_DOT_ONE = 2 << 8 | 1;
         const short REQUESTED_CONTEXT = attribList[1] << 8 | attribList[2];

         // if the requested context is a 2.1 or lower,
         // remove the profile mask and the context flags
         // as these types of contexts do not use or need these...
         if (REQUESTED_CONTEXT <= CONTEXT_TWO_DOT_ONE)
         {
            attribList[4] = 0;
            attribList[5] = 0;
            attribList[6] = 0;
            attribList[7] = 0;
         }

         mGLContext =
            OpenGLExt::wglCreateContextAttribsARB(hDC, NULL, attribList);
      }
   }
   else
   {
      // use the basic context for opengl requests
      mGLContext = basicContext;
   }

   return mGLContext ? true : false;
}

void OpenGLWindow::ReleaseOpenGLContext( )
{
   // release the current context
   ReleaseCurrent();
   // release the context
   wglDeleteContext(mGLContext);
}