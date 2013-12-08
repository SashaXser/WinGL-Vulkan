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
      bool bEnableDebug;
      bool bEnableForwardCompatibleContext;
   };

   // creates the application
   virtual bool Create( unsigned int nWidth,
                        unsigned int nHeight,
                        const char * pWndTitle,
                        const void ** pInitParams = NULL );

protected:
   // destructor...
   virtual ~OpenGLWindow( );

   // called when the window is about to be destroyed
   // the opengl context is no longer valid after this call returns
   virtual void OnDestroy( ) override;

   // obtain access to the render context
   HGLRC GetGLContext( ) { return mGLContext; }

   // makes the render context current
   BOOL MakeCurrent( ) { return wglMakeCurrent(GetDC(GetHWND()), mGLContext); }
   BOOL ReleaseCurrent( ) { return wglMakeCurrent(GetDC(GetHWND()), NULL); }

   // attaches debug out to the application...
   // a debug context must be requested for this to work...
   bool AttachToDebugContext( );
   void DetachFromDebugContext( );

private:
   // prohibit copy constructor
   OpenGLWindow( const OpenGLWindow & );
   // prohibit copy operator
   OpenGLWindow & operator = ( const OpenGLWindow & );

   // private static debug context callbacks
   static void __stdcall DebugContextCallbackAMD( unsigned int id,
                                                  unsigned int category,
                                                  unsigned int severity,
                                                  int length,
                                                  const char * message,
                                                  void * userParams );
   static void __stdcall DebugContextCallbackARB( unsigned int source,
                                                  unsigned int type,
                                                  unsigned int id,
                                                  unsigned int severity,
                                                  int length,
                                                  const char * message,
                                                  void * userParams );

   // creates an OpenGL context
   bool CreateOpenGLContext( const OpenGLInit ** pInitParams );

   // releases the OpenGL context
   void ReleaseOpenGLContext( );

   // private member variables
   HGLRC    mGLContext;

   // determines if gl context requested debug mode
   bool     mDebugRequested;

};

#endif // _OPENGL_WINDOW_H_