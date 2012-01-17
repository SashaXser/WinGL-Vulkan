#ifndef _RENDER_TARGET_WINDOW_H_
#define _RENDER_TARGET_WINDOW_H_

// wingl includes
#include "OpenGLWindow.h"

class RenderTargetWindow : public OpenGLWindow
{
public:
   // constructor
   // destructor is not public to make sure
   // that the class must be destroyed internally
   // through message WM_NCDESTROY...
            RenderTargetWindow( );

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
   virtual ~RenderTargetWindow( );

   // handles messages passed by the system
   virtual LRESULT MessageHandler( UINT uMsg,
                                   WPARAM wParam,
                                   LPARAM lParam );

private:
   // setup the scene state
   void SetupScene( );

   // creates the render target
   void CreateRenderTarget( );

   // renders the scene
   void RenderScene( );

   // renders into the texture
   void RenderTexture( );

   // private member variables
   unsigned int      mFrameBuffer;
   unsigned int      mClrAttachTex;
   unsigned int      mClrAttachTexWH[2];

};

#endif // _RENDER_TARGET_WINDOW_H_
