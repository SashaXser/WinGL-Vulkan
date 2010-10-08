#ifndef _OPENGL_WINDOW_H_
#define _OPENGL_WINDOW_H_

// local includes
#include "Window.h"

class OpenGLWindow : public Window
{
public:
   // constructor
   // destructor is not public to make sure
   // that the class must be destroyed internally
   // through message WM_NCDESTROY...
            OpenGLWindow( );

   // initialization structure for the create function
   struct OpenGLInit
   {
      int nMajorVer;
      int nMinorVer;
      bool bCompatibleContext;
   };

   // creates the application
   virtual bool Create( unsigned int nWidth,
                        unsigned int nHeight,
                        const char * pWndTitle,
                        const void * pInitParams = NULL );

protected:
   // destructor...
   virtual ~OpenGLWindow( );

   // obtain access to the render context
   HGLRC GetGLContext( ) { return mGLContext; }

   // makes the render context current
   BOOL MakeCurrent( ) { return wglMakeCurrent(GetDC(GetHWND()), mGLContext); }
   BOOL ReleaseCurrent( ) { return wglMakeCurrent(GetDC(GetHWND()), NULL); }

private:
   // prohibit copy constructor
            OpenGLWindow( const OpenGLWindow & );
   // prohibit copy operator
   OpenGLWindow & operator = ( const OpenGLWindow & );

   // creates an OpenGL context
   bool CreateOpenGLContext( const OpenGLInit * pInitParams );

   // releases the OpenGL context
   void ReleaseOpenGLContext( );

   // private member variables
   HGLRC    mGLContext;

};

#endif // _OPENGL_WINDOW_H_