// local includes
#include "Window.h"
#include "WglAssert.h"

// window includes
#include <MMSystem.h>

// std includes
#include <cstdint>

// library includes
#pragma comment( lib, "Winmm.lib" )

Window::Window( ) :
mHDC           ( NULL ),
mHWND          ( NULL ),
mPrevMousePos  ( { 0, 0 } )
{
   // set the min timer resolution
   timeBeginPeriod(1);
}

Window::~Window( )
{
   // reset the min timer resolution
   timeEndPeriod(1);
}

bool Window::Create( unsigned int nWidth,
                     unsigned int nHeight,
                     const char * pWndTitle,
                     const void * pInitParams )
{
   // register the class window
   WNDCLASSEX wndClassEx =
   {
      sizeof(wndClassEx),
      CS_DBLCLKS | CS_HREDRAW | CS_OWNDC | CS_VREDRAW,
      &Window::WindowProcedure,
      0, sizeof(this),
      reinterpret_cast< HINSTANCE >(GetCurrentProcess()),
      NULL,
      LoadCursor(NULL, IDC_ARROW),
      NULL, NULL,
      "BasicWindow",
      NULL
   };

   // register the class
   if (ATOM classAtom = RegisterClassEx(&wndClassEx))
   {
      // window styles and styles extra
      const DWORD nStyles = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW;
      const DWORD nStylesEx = WS_EX_OVERLAPPEDWINDOW;

      // adjust the window to account for the borders...
      RECT clientRect = { 0, 0, nWidth, nHeight };
      AdjustWindowRectEx(&clientRect, nStyles, FALSE, nStylesEx);

      // create the window
      mHWND =
         CreateWindowEx(nStylesEx,
                        reinterpret_cast< LPCSTR >(classAtom),
                        pWndTitle,
                        nStyles,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        static_cast< int >(clientRect.right - clientRect.left),
                        static_cast< int >(clientRect.bottom - clientRect.top),
                        NULL,
                        NULL,
                        reinterpret_cast< HINSTANCE >(GetCurrentProcess()),
                        this);

      if (mHWND)
      {
         // obtain the dc for the hwnd
         mHDC = GetDC(mHWND);

         // embed this into the window instance
         SetWindowLongPtr(mHWND,
                          GWLP_USERDATA,
                          reinterpret_cast< LONG_PTR >(this));

         // make the window visible
         ShowWindow(mHWND, SW_SHOW);

         return true;
      }
   }

   return false;
}

Size Window::GetSize( )
{
   RECT clientRect;
   GetClientRect(mHWND, &clientRect);

   Size clientSize =
   {
      clientRect.right - clientRect.left,
      clientRect.bottom - clientRect.top
   };

   return clientSize;
}

int Window::Run( )
{
   int retValue = 0;
   while (!GetAppMessages(retValue));

   return retValue;
}

bool Window::GetAppMessages( int & nQuitRetValue )
{
   MSG wndMsg;
   BOOL retVal = 0;

   while ((retVal = GetMessage(&wndMsg, NULL, 0, 0)) != 0)
   {
      if (retVal == -1)
      {
         // there is an error...
         // silently ignore it...
         WGL_ASSERT(false);
      }
      else
      {
         // translate and route the message
         TranslateMessage(&wndMsg);
         DispatchMessage(&wndMsg);
      }
   }

   // make sure that the message is the quit message...
   // assign the quit message return value...
   WGL_ASSERT(wndMsg.message == WM_QUIT);
   nQuitRetValue = wndMsg.wParam & 0x00000000FFFFFFFF;

   return true;
}

bool Window::PeekAppMessages( int & nQuitRetValue )
{
   MSG wndMsg;
   bool termApp = false;

   while (PeekMessage(&wndMsg, NULL, 0, 0, PM_REMOVE) != 0)
   {
      if (wndMsg.message == WM_QUIT)
      {
         // application has requtested termination
         // indicate termination to the calling function
         termApp = true;
         nQuitRetValue = wndMsg.wParam & 0x00000000FFFFFFFF;
         break;
      }
      else
      {
         // translate and route the message
         TranslateMessage(&wndMsg);
         DispatchMessage(&wndMsg);
      }
   }

   return termApp;
}

LRESULT Window::MessageHandler( UINT uMsg,
                                WPARAM wParam,
                                LPARAM lParam )
{
   LRESULT result = 0;

   switch (uMsg)
   {
   case WM_CLOSE:
      // allow destroy window to be called
      result = ::DefWindowProc(mHWND, uMsg, wParam, lParam);
      // the window is no longer valid
      PostQuitMessage(0);

      break;

   case WM_DESTROY:
      // all the container window to process
      OnDestroy();
      // the window is still valid before and after this call
      result = ::DefWindowProc(mHWND, uMsg, wParam, lParam);

      break;

   case WM_NCDESTROY:
      // delete the container window
      delete this;
      // the window is still valid before and after this call
      result = ::DefWindowProc(mHWND, uMsg, wParam, lParam);

      break;

   default:
      result = ::DefWindowProc(mHWND, uMsg, wParam, lParam);

      break;
   }

   return result;
}

LRESULT Window::WindowProcedure( HWND hWnd,
                                 UINT uMsg,
                                 WPARAM wParam,
                                 LPARAM lParam )
{
   LRESULT result = 0;

   Window * const pWindow = 
      reinterpret_cast< Window * >(GetWindowLongPtr(hWnd, GWLP_USERDATA));

   if (pWindow)
   {
      result = pWindow->MessageHandler(uMsg, wParam, lParam);

      if (uMsg == WM_MOUSEMOVE)
      {
         pWindow->mPrevMousePos.x = static_cast< intptr_t >(lParam & 0xFFFF);
         pWindow->mPrevMousePos.y = static_cast< intptr_t >(lParam >> 16);
      }
   }
   else
   {
      result = ::DefWindowProc(hWnd, uMsg, wParam, lParam);
   }

   return result;
}
