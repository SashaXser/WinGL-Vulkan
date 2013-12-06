#ifndef _INSTANCING_WINDOW_H_
#define _INSTANCING_WINDOW_H_

// local includes

// wingl includdes
#include "OpenGLWindow.h"

// std includes
#include <cstdint>

class WindowlessRenderWindow : public OpenGLWindow
{
public:
   // constructor...
   // destructor is not public to make sure
   // that the class must be destroyed internally
   // through message WM_NCDESTROY...
   WindowlessRenderWindow( );

   // creates the application
   virtual bool Create( unsigned int nWidth, unsigned int nHeight,
                        const char * pWndTitle, const void ** pInitParams = nullptr );

   // basic run implementation
   // will process messages until app quit
   virtual int Run( );

protected:
   // destructor...
   virtual ~WindowlessRenderWindow( );

   // handles messages passed by the system
   virtual LRESULT MessageHandler( UINT uMsg, WPARAM wParam, LPARAM lParam );

private:
   // prohibit copy construction
   WindowlessRenderWindow( const WindowlessRenderWindow & );
   // prohibit copy operator
   WindowlessRenderWindow & operator = ( const WindowlessRenderWindow & );

   // renders the scene
   void RenderScene( );

   // entry point for the server thread
   uint32_t RunServerThread( );
   static uint32_t __stdcall ServerThreadEntry( void * pData );   

   // handle to the server thread
   uintptr_t   mServerThread;

};

#endif // _INSTANCING_WINDOW_H_
