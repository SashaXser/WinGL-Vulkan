// local includes
#include "OpenGLExtensions.h"

// gl includes
#include <gl/gl.h>

namespace gl
{

HWND CreateOpenGLWindow( const HWND wnd, HGLRC & context )
{
   // the temp opengl window
   HWND tempWnd = wnd;

   if (!tempWnd)
   {
      // register a temp window
      WNDCLASSEX wndClassEx =
      {
         sizeof(wndClassEx),
         CS_DBLCLKS | CS_HREDRAW | CS_OWNDC | CS_VREDRAW,
         &DefWindowProc,
         0, 0,
         reinterpret_cast< HINSTANCE >(GetCurrentProcess()),
         nullptr, nullptr, nullptr, nullptr,
         "temp-basic-opengl-window-class",
         nullptr
      };

      // get back an atom from the registration process
      if (const ATOM atom = RegisterClassEx(&wndClassEx))
      {
         // window styles and styles extra
         const DWORD nStyles = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW;
         const DWORD nStylesEx = WS_EX_OVERLAPPEDWINDOW;

         // create the window
         if (!(tempWnd = CreateWindowEx(nStylesEx,
                                        reinterpret_cast< LPCSTR >(atom),
                                        "temp-basic-opengl-window",
                                        nStyles,
                                        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                        nullptr, nullptr,
                                        reinterpret_cast< HINSTANCE >(GetCurrentProcess()),
                                        nullptr)))
         {
            // unregister the class
            UnregisterClass(wndClassEx.lpszClassName,
                            reinterpret_cast< HINSTANCE >(GetCurrentProcess()));
         }
      }
   }

   if (tempWnd)
   {
      // create a pixel format descriptor
      PIXELFORMATDESCRIPTOR pfd = { 0 };
      
      // setup basic attributes
      pfd.nSize = sizeof(pfd);
      pfd.nVersion = 1;
      pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_SUPPORT_COMPOSITION;
      pfd.iPixelType = PFD_TYPE_RGBA;
      pfd.cColorBits = 32;
      pfd.cDepthBits = 24;
      pfd.iLayerType = PFD_MAIN_PLANE;
      
      // obtain the pixel format
      const HDC dc = GetDC(tempWnd);
      const int format = ChoosePixelFormat(dc, &pfd);
      
      // set the pixel format
      SetPixelFormat(dc, format, &pfd);
      
      // create the basic opengl context
      context = wglCreateContext(dc);
   }

   return tempWnd;
}

bool IsExtensionSupported( const char * const pExtension )
{
   // currently not supported
   bool supported = false;

   // obtain the extensions string
   const char * pExtensions =
      reinterpret_cast< const char * >(glGetString(GL_EXTENSIONS));

   // obtain the extensions str length
   const size_t extStrLen = strlen(pExtension);

   // continue until no extension is found
   while (pExtensions)
   {
      // substring into the list of extensions
      pExtensions = strstr(pExtensions, pExtension);

      // make sure an extension is found and that the
      // next character is a space or the null terminator
      if (pExtensions && (*(pExtensions + extStrLen) == ' ' ||
                          *(pExtensions + extStrLen) == '\0'))
      {
         supported = true; break;
      }
   }
   
   return supported;
}

} // namespace gl