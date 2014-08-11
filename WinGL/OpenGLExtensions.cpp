// local includes
#include "OpenGLExtensions.h"
#include "WglAssert.h"

// gl includes
#include "GL/glew.h"
#include <gl/gl.h>

// std includes
#include <string>
#include <cstring>

namespace gl
{

const uint32_t VERSION_1_0 = 0x00010000;
const uint32_t VERSION_1_1 = 0x00010001;
const uint32_t VERSION_2_0 = 0x00020000;
const uint32_t VERSION_2_1 = 0x00020001;
const uint32_t VERSION_3_0 = 0x00030000;
const uint32_t VERSION_3_1 = 0x00030001;
const uint32_t VERSION_3_2 = 0x00030002;
const uint32_t VERSION_3_3 = 0x00030003;
const uint32_t VERSION_4_0 = 0x00040000;
const uint32_t VERSION_4_1 = 0x00040001;
const uint32_t VERSION_4_2 = 0x00040002;
const uint32_t VERSION_4_3 = 0x00040003;
const uint32_t VERSION_4_4 = 0x00040004;

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

   if (GetVersion() >= VERSION_3_0)
   {
      // obtain the total number of extensions
      const GLint gl_num_ext = [ ] ( ) -> GLint
      {
         GLint num_ext = 0;
         glGetIntegerv(GL_NUM_EXTENSIONS, &num_ext);

         return num_ext;
      }();

      for (GLint i = 0; i < gl_num_ext && !supported; ++i)
      {
         // obtain the extension name at the specified index
         const char * const pObtainedExt =
            reinterpret_cast< const char * >(glGetStringi(GL_EXTENSIONS, i));

         // do a comparison of the obtained to the requested
         supported = std::strcmp(pObtainedExt, pExtension) == 0;
      }
   }
   else
   {
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
   }
   
   return supported;
}

uint32_t GetVersion_i( )
{
   // obtain the string representing the version
   const char * const pGLVersion =
      reinterpret_cast< const char * >(glGetString(GL_VERSION));

   WGL_ASSERT(pGLVersion);

   // stuff the version into a temp buffer
   char gl_version[128] = { 0 };
   std::strncpy(gl_version, pGLVersion, sizeof(gl_version) - 1);

   // obtaint the major version
   const uint32_t major = std::stoul(std::strtok(gl_version, "."));
   const uint32_t minor = std::stoul(std::strtok(nullptr, ". "));

   return major << 16 | minor;
}

uint32_t GetVersion( )
{
   static const uint32_t VERSION = GetVersion_i();

   WGL_ASSERT(GetVersion_i() == VERSION);

   return VERSION;
}

} // namespace gl