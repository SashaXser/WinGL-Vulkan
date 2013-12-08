#ifndef _WINDOW_H_
#define _WINDOW_H_

// platform includes
#include <windows.h>

// local includes
#include "WindowDefines.h"

class Window
{
public:
   // constructor
   // destructor is not public to make sure
   // that the class must be destroyed internally
   // through message WM_NCDESTROY...
   Window( );

   // creates the application
   virtual bool Create( unsigned int nWidth,
                        unsigned int nHeight,
                        const char * pWndTitle,
                        const void * pInitParams = NULL );

   // returns the hwnd object
   const HWND &   GetHWND( ) const { return mHWND; }

   // returns the dc object
   const HDC &    GetHDC( ) const { return mHDC; }

   // returns the size of the window
   Size  GetSize( );

   // basic run implementation
   // will process messages until app quit
   virtual int Run( );

protected:
   // destructor...
   virtual ~Window( );

   // called when the window is about to be destroyed
   // the opengl context is no longer valid after this call returns
   virtual void OnDestroy( ) { }

   // handles messages passed by the system
   virtual LRESULT MessageHandler( UINT uMsg,
                                   WPARAM wParam,
                                   LPARAM lParam );

   // processes app messages
   bool GetAppMessages( int & nQuitRetValue );
   bool PeekAppMessages( int & nQuitRetValue );

   // protected member variables
   HDC      mHDC;
   HWND     mHWND;

private:
   // prohibit copy constructor
            Window( const Window & );
   // prohibit copy operator
   Window & operator = ( const Window & );

   // static message pump to handle messages
   static LRESULT CALLBACK WindowProcedure( HWND hWnd,
                                            UINT uMsg,
                                            WPARAM wParam,
                                            LPARAM lParam );

};

#endif // _WINDOW_H_