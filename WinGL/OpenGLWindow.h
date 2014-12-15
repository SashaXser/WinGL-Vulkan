#ifndef _OPENGL_WINDOW_H_
#define _OPENGL_WINDOW_H_

// local includes
#include "Window.h"

// std includes
#include <cstdint>

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
                        const OpenGLInit * const pInitParams = NULL );

protected:
   // destructor...
   virtual ~OpenGLWindow( );

   // called when the window is about to be destroyed
   virtual void OnDestroy( ) override;

   // obtain access to the render context
   HGLRC GetGLContext( ) { return mGLContext; }

   // makes the render context current
   BOOL MakeCurrent( ) { return wglMakeCurrent(GetDC(GetHWND()), mGLContext); }
   BOOL ReleaseCurrent( ) { return wglMakeCurrent(GetDC(GetHWND()), NULL); }

   // determines if the context is current
   BOOL ContextIsCurrent( ) const { return wglGetCurrentContext() == mGLContext; }

   // attaches debug out to the application...
   // a debug context must be requested for this to work...
   bool AttachToDebugContext( );
   void DetachFromDebugContext( );

   // allow for inserts into the debug message structure
   void PostDebugMessage( const uint32_t type,
                          const uint32_t id,
                          const uint32_t severity,
                          const char * const pMsg );

private:
   // prohibit copy constructor
   OpenGLWindow( const OpenGLWindow & );
   // prohibit copy operator
   OpenGLWindow & operator = ( const OpenGLWindow & );

   // private static debug context callbacks
   static void __stdcall DebugContextCallbackAMD( uint32_t id,
                                                  uint32_t category,
                                                  uint32_t severity,
                                                  int32_t length,
                                                  const char * message,
                                                  void * userParams );
   static void __stdcall DebugContextCallbackARB( uint32_t source,
                                                  uint32_t type,
                                                  uint32_t id,
                                                  uint32_t severity,
                                                  int32_t length,
                                                  const char * message,
                                                  const void * userParams );

   // creates an OpenGL context
   bool CreateOpenGLContext( const OpenGLInit * pInitParams );

   // releases the OpenGL context
   void ReleaseOpenGLContext( );

   // private member variables
   HGLRC    mGLContext;

   // determines if gl context requested debug mode
   bool     mDebugRequested;

};

#endif // _OPENGL_WINDOW_H_