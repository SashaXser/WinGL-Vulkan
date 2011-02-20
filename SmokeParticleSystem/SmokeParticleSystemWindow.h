#ifndef _SMOKE_PARTICLE_SYSTEM_WINDOW_H_
#define _SMOKE_PARTICLE_SYSTEM_WINDOW_H_

// local includes
#include "OpenGLWindow.h"

// forward declarations
struct SimFrame;

class SmokeParticleSystemWindow : public OpenGLWindow
{
public:
   // constructor
   // destructor is not public to make sure
   // that the class must be destroyed internally
   // through message WM_NCDESTROY...
            SmokeParticleSystemWindow( );

   // creates the application
   virtual bool Create( unsigned int nWidth,
                        unsigned int nHeight,
                        const char * pWndTitle,
                        const void * pInitParams = NULL );

   // basic run implementation
   // will process messages until app quit
   virtual int Run( );
   
protected:
   // destructor
   virtual ~SmokeParticleSystemWindow( );

   // handles messages passed by the system
   virtual LRESULT MessageHandler( UINT uMsg,
                                   WPARAM wParam,
                                   LPARAM lParam );

private:

   // rendering functions
   void RenderScene( const SimFrame & rSimFrame );
   void RenderFloor( const SimFrame & simFrame,
                     const bool shutdown = false );

   // called to update the scene
   void UpdateScene( const SimFrame & rSimFrame );

   // gets the sim frame time
   void GetSimFrameTime( SimFrame & simFrame );

};

#endif // _SMOKE_PARTICLE_SYSTEM_WINDOW_H_