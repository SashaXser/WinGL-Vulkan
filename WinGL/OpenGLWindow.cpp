// local includes
#include "OpenGLWindow.h"
#include "OpenGLExtensions.h"
#include "WglAssert.h"

// opengl includes
#include <GL/glew.h>
#include <GL/wglew.h>
#include <gl/gl.h>

// stl includes
#include <sstream>
#include <iostream>

// helper function to obtain a function pointer
template < typename F >
void ObtainGLEntryPoint( const char * const pName, F & f )
{
   f = reinterpret_cast< F >(wglGetProcAddress(pName));
}

OpenGLWindow::OpenGLWindow( ) :
mGLContext        ( NULL ),
mDebugRequested   ( false )
{
}

OpenGLWindow::~OpenGLWindow( )
{
}

bool OpenGLWindow::Create( unsigned int nWidth,
                           unsigned int nHeight,
                           const char * pWndTitle,
                           const void ** pInitParams )
{
   // create the main window
   if (Window::Create(nWidth, nHeight, pWndTitle, NULL))
   {
      // create the opengl context
      return CreateOpenGLContext(reinterpret_cast< const OpenGLInit ** >(pInitParams));
   }

   return false;
}

void OpenGLWindow::OnDestroy( )
{
   // detach from the debug context...
   // if one was not created or not requested,
   // then the function just ignores this operation...
   DetachFromDebugContext();
   // release the gl context...
   ReleaseOpenGLContext();

   // allow base class to cleanup
   Window::OnDestroy();
}

bool OpenGLWindow::CreateOpenGLContext( const OpenGLInit ** pInitParams )
{
   if (pInitParams)
   {
      // create a basic gl context
      HGLRC basicContext = nullptr;
      if (const HWND basicWnd = gl::CreateOpenGLWindow(nullptr, basicContext))
      {
         // get the basic dc
         const HDC basicDC = GetDC(basicWnd);

         // make basic current
         wglMakeCurrent(basicDC, basicContext);

         // obtain the required function pointers
         PFNWGLCHOOSEPIXELFORMATARBPROC __wglChoosePixelFormatARB = nullptr;
         PFNWGLCREATECONTEXTATTRIBSARBPROC __wglCreateContextAttribsARB = nullptr;
         ObtainGLEntryPoint("wglChoosePixelFormatARB", __wglChoosePixelFormatARB);
         ObtainGLEntryPoint("wglCreateContextAttribsARB", __wglCreateContextAttribsARB);

         // choose a pixel format first
         const GLint iAttribs[] =
         {
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
            WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
            WGL_SWAP_METHOD_ARB,    WGL_SWAP_EXCHANGE_ARB,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
            WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
            WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
            WGL_COLOR_BITS_ARB,     32,
            WGL_DEPTH_BITS_ARB,     24,
            WGL_STENCIL_BITS_ARB,   8,
            0,                      0
         };

         GLint format = 0; GLuint totalFormats = 0;
         __wglChoosePixelFormatARB(mHDC, iAttribs, nullptr, 1, &format, &totalFormats);

         // set the specified pixel format
         // make sure that composition support is turned on
         PIXELFORMATDESCRIPTOR pfd = { 0 };
         DescribePixelFormat(mHDC, format, sizeof(pfd), &pfd);
         pfd.dwFlags |= PFD_SUPPORT_COMPOSITION;
         SetPixelFormat(mHDC, format, &pfd);

         for (; !mGLContext && *pInitParams; ++pInitParams)
         {
            // create a new context
            int attribList[] =
            {
               WGL_CONTEXT_MAJOR_VERSION_ARB,   0,
               WGL_CONTEXT_MINOR_VERSION_ARB,   0,
               WGL_CONTEXT_PROFILE_MASK_ARB,    0,
               WGL_CONTEXT_FLAGS_ARB,           0,
               0,                               0
            };

            // create the specified context...
            attribList[1] = (*pInitParams)->nMajorVer;
            attribList[3] = (*pInitParams)->nMinorVer;
            attribList[5] = (*pInitParams)->bCompatibleContext ?
                            WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB :
                            WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
            attribList[7] = (*pInitParams)->bEnableDebug ?
                            WGL_CONTEXT_DEBUG_BIT_ARB : 0;
            attribList[7] |= (*pInitParams)->bEnableForwardCompatibleContext ?
                             WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB : 0;

            // save the debug flag
            mDebugRequested = (*pInitParams)->bEnableDebug;

            // store a 2.1 value and the requested
            // context in two byte of data
            const short CONTEXT_TWO_DOT_ONE = 2 << 8 | 1;
            const short REQUESTED_CONTEXT = attribList[1] << 8 | attribList[2];

            // if the requested context is a 2.1 or lower,
            // remove the profile mask and the context flags
            // as these types of contexts do not use or need these...
            // debug context not valid for 2.1 or lower...
            if (REQUESTED_CONTEXT <= CONTEXT_TWO_DOT_ONE)
            {
               attribList[4] = 0;
               attribList[5] = 0;
               attribList[6] = 0;
               attribList[7] = 0;

               mDebugRequested = false;
            }

            mGLContext = __wglCreateContextAttribsARB(mHDC, NULL, attribList);
         }

         // release the basic context
         wglMakeCurrent(basicDC, NULL);

         // destroy the basic context
         wglDeleteContext(basicContext);

         // destroy the basic window
         DestroyWindow(basicWnd);

         // unregister the basic window class
         char basicClassName[1024] = { 0 };
         GetClassName(basicWnd, basicClassName, sizeof(basicClassName));
         UnregisterClass(basicClassName, reinterpret_cast< HINSTANCE >(GetCurrentProcess()));
      }
   }
   else
   {
      // create a basic context for the window
      gl::CreateOpenGLWindow(mHWND, mGLContext);
   }

   // if there is a context, then init glew
   if (mGLContext)
   {
      MakeCurrent();
      glewInit();
      ReleaseCurrent();
   }

   return mGLContext ? true : false;
}

void OpenGLWindow::ReleaseOpenGLContext( )
{
   // release the current context
   ReleaseCurrent();
   // release the context
   wglDeleteContext(mGLContext);
}

bool OpenGLWindow::AttachToDebugContext( )
{
   bool attached = false;

   // make sure that the debug context was requsted
   if (mDebugRequested)
   {
      // check to see if the arb is supported...
      // if the arb is not supported, try the amd extensions...
      if (gl::IsExtensionSupported("GL_ARB_debug_output"))
      {
         WGL_ASSERT(!"OpenGLWindow::DetachFromDebugContext - Needs implementation");
      }
      else if (gl::IsExtensionSupported("GL_AMD_debug_output"))
      {
         // amd extension is supported here...
         // make the callback into the static function and pass this as the param...
         glDebugMessageCallbackAMD(&OpenGLWindow::DebugContextCallbackAMD, this);

         // enable all errors at the highest settings...
         unsigned int pDebugCat[] =
         {
            GL_DEBUG_CATEGORY_API_ERROR_AMD,
            GL_DEBUG_CATEGORY_WINDOW_SYSTEM_AMD,
            GL_DEBUG_CATEGORY_DEPRECATION_AMD,
            GL_DEBUG_CATEGORY_UNDEFINED_BEHAVIOR_AMD,
            GL_DEBUG_CATEGORY_PERFORMANCE_AMD,
            GL_DEBUG_CATEGORY_SHADER_COMPILER_AMD,
            GL_DEBUG_CATEGORY_APPLICATION_AMD,
            GL_DEBUG_CATEGORY_OTHER_AMD
         };

         for (unsigned int i = 0; i < sizeof(pDebugCat) / sizeof(*pDebugCat); ++i)
         {
            glDebugMessageEnableAMD(*(pDebugCat + i), GL_DEBUG_SEVERITY_HIGH_AMD, 0, NULL, GL_TRUE);
            glDebugMessageEnableAMD(*(pDebugCat + i), GL_DEBUG_SEVERITY_MEDIUM_AMD, 0, NULL, GL_TRUE);
            glDebugMessageEnableAMD(*(pDebugCat + i), GL_DEBUG_SEVERITY_LOW_AMD, 0, NULL, GL_TRUE);
         }

         attached = true;
      }
   }

   return attached;
}

void OpenGLWindow::DetachFromDebugContext( )
{
   // make sure that a debug context was requested...
   if (mDebugRequested)
   {
      // check to see if the arb is supported...
      // if the arb is not supported, try the amd extensions...
      if (gl::IsExtensionSupported("GL_ARB_debug_output"))
      {
         WGL_ASSERT(!"OpenGLWindow::DetachFromDebugContext - Needs implementation"); 
      }
      else if (gl::IsExtensionSupported("GL_AMD_debug_output"))
      {
         // amd extension is supported here...
         // disable the callback by setting null to both params
         glDebugMessageCallbackAMD(NULL, NULL);

         // disable all errors at the highest settings...
         unsigned int pDebugCat[] =
         {
            GL_DEBUG_CATEGORY_API_ERROR_AMD,
            GL_DEBUG_CATEGORY_WINDOW_SYSTEM_AMD,
            GL_DEBUG_CATEGORY_DEPRECATION_AMD,
            GL_DEBUG_CATEGORY_UNDEFINED_BEHAVIOR_AMD,
            GL_DEBUG_CATEGORY_PERFORMANCE_AMD,
            GL_DEBUG_CATEGORY_SHADER_COMPILER_AMD,
            GL_DEBUG_CATEGORY_APPLICATION_AMD,
            GL_DEBUG_CATEGORY_OTHER_AMD
         };

         for (unsigned int i = 0; i < sizeof(pDebugCat) / sizeof(*pDebugCat); ++i)
         {
            glDebugMessageEnableAMD(*(pDebugCat + i), GL_DEBUG_SEVERITY_HIGH_AMD, 0, NULL, GL_FALSE);
            glDebugMessageEnableAMD(*(pDebugCat + i), GL_DEBUG_SEVERITY_MEDIUM_AMD, 0, NULL, GL_FALSE);
            glDebugMessageEnableAMD(*(pDebugCat + i), GL_DEBUG_SEVERITY_LOW_AMD, 0, NULL, GL_FALSE);
         }
      }
   }
}

void __stdcall OpenGLWindow::DebugContextCallbackAMD( unsigned int id,
                                                      unsigned int category,
                                                      unsigned int severity,
                                                      int length,
                                                      const char * message,
                                                      void * userParams )
{
   // message string to output
   std::stringstream ssMsg;

   // put the pieces together
   ssMsg << "DebugContextCallbackAMD" << std::endl
         << "ID: " << id << " - CATEGORY: ";

   switch (category)
   {
   case GL_DEBUG_CATEGORY_API_ERROR_AMD:           ssMsg << "API"; break;
   case GL_DEBUG_CATEGORY_WINDOW_SYSTEM_AMD:       ssMsg << "Window"; break;
   case GL_DEBUG_CATEGORY_DEPRECATION_AMD:         ssMsg << "Depricated"; break;
   case GL_DEBUG_CATEGORY_UNDEFINED_BEHAVIOR_AMD:  ssMsg << "Undef Behavior"; break;
   case GL_DEBUG_CATEGORY_PERFORMANCE_AMD:         ssMsg << "Performance"; break;
   case GL_DEBUG_CATEGORY_SHADER_COMPILER_AMD:     ssMsg << "Shader Compiler"; break;
   case GL_DEBUG_CATEGORY_APPLICATION_AMD:         ssMsg << "App"; break;
   case GL_DEBUG_CATEGORY_OTHER_AMD:               ssMsg << "Other"; break;
   }

   ssMsg << " - SEVERITY: ";

   switch (severity)
   {
   case GL_DEBUG_SEVERITY_HIGH_AMD:    ssMsg << "HIGH"; break;
   case GL_DEBUG_SEVERITY_MEDIUM_AMD:  ssMsg << "MEDIUM"; break;
   case GL_DEBUG_SEVERITY_LOW_AMD:     ssMsg << "LOW"; break;
   }

   ssMsg << std::endl << "MSG: " << message << std::endl;

   // output to the debug out stream
   std::cout << ssMsg.str();
}

void __stdcall OpenGLWindow::DebugContextCallbackARB( unsigned int source,
                                                      unsigned int type,
                                                      unsigned int id,
                                                      unsigned int severity,
                                                      int length,
                                                      const char * message,
                                                      void * userParams )
{
}