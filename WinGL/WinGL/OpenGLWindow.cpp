// local includes
#include "OpenGLWindow.h"
#include "OpenGLExtensions.h"
#include "WglAssert.h"

// opengl includes
#include <gl/gl.h>

// stl includes
#include <sstream>

OpenGLWindow::OpenGLWindow( ) :
mGLContext        ( NULL ),
mDebugRequested   ( false )
{
}

OpenGLWindow::~OpenGLWindow( )
{
   // detach from the debug context...
   // if one was not created or not requested,
   // then the function just ignores this operation...
   DetachFromDebugContext();
   // release the gl context...
   ReleaseOpenGLContext();
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

bool OpenGLWindow::CreateOpenGLContext( const OpenGLInit ** pInitParams )
{
   // create a pixel format descriptor
   PIXELFORMATDESCRIPTOR pfd;

   // clear out the pfd
   memset(&pfd, 0x00, sizeof(pfd));

   // setup basic attributes
   pfd.nSize = sizeof(pfd);
   pfd.nVersion = 1;
   pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
   pfd.iPixelType = PFD_TYPE_RGBA;
   pfd.cColorBits = 24;
   pfd.cAlphaBits = 8;
   pfd.cDepthBits = 32;
   pfd.iLayerType = PFD_MAIN_PLANE;

   // obtain the pixel format
   const HDC hDC = GetHDC();
   const int pixelFormat = ChoosePixelFormat(hDC, &pfd);

   // set the pixel format
   SetPixelFormat(hDC, pixelFormat, &pfd);

   // create the basic opengl context
   const HGLRC basicContext = wglCreateContext(hDC);

   // make basic current
   wglMakeCurrent(hDC, basicContext);

   // initialize the opengl extensions
   // need to reinterpret the function sig to make x64 happy...
   OpenGLExt::InitializeOpenGLExtensions(
      reinterpret_cast< int (* (__stdcall *)( const char * ))( void ) >
      (&wglGetProcAddress));

   // release the basic context
   wglMakeCurrent(hDC, NULL);

   if (pInitParams)
   {
      // destroy the basic context
      wglDeleteContext(basicContext);

      for (; !mGLContext && *pInitParams; ++pInitParams)
      {
         // create a new context
         int attribList[] =
         {
            OpenGLExt::WGL_CONTEXT_MAJOR_VERSION_ARB, 0,
            OpenGLExt::WGL_CONTEXT_MINOR_VERSION_ARB, 0,
            OpenGLExt::WGL_CONTEXT_PROFILE_MASK_ARB, 0,
            OpenGLExt::WGL_CONTEXT_FLAGS_ARB, 0,
            0, 0
         };
         
         // create the specified context...
         attribList[1] = (*pInitParams)->nMajorVer;
         attribList[3] = (*pInitParams)->nMinorVer;
         attribList[5] = (*pInitParams)->bCompatibleContext ?
                         OpenGLExt::WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB :
                         OpenGLExt::WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
         attribList[7] = (*pInitParams)->bEnableDebug ?
                         OpenGLExt::WGL_CONTEXT_DEBUG_BIT_ARB : 0;
         attribList[7] |= (*pInitParams)->bEnableForwardCompatibleContext ?
                         OpenGLExt::WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB : 0;

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

         mGLContext =
            OpenGLExt::wglCreateContextAttribsARB(hDC, NULL, attribList);
      }
   }
   else
   {
      // use the basic context for opengl requests
      mGLContext = basicContext;
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
      if (OpenGLExt::IsExtensionSupported("GL_ARB_debug_output"))
      {
         WGL_ASSERT(!"OpenGLWindow::DetachFromDebugContext - Needs implementation");
      }
      else if (OpenGLExt::IsExtensionSupported("GL_AMD_debug_output"))
      {
         // amd extension is supported here...
         // make the callback into the static function and pass this as the param...
         OpenGLExt::glDebugMessageCallbackAMD(&OpenGLWindow::DebugContextCallbackAMD, this);

         // enable all errors at the highest settings...
         unsigned int pDebugCat[] =
         {
            OpenGLExt::GL_DEBUG_CATEGORY_API_ERROR_AMD,
            OpenGLExt::GL_DEBUG_CATEGORY_WINDOW_SYSTEM_AMD,
            OpenGLExt::GL_DEBUG_CATEGORY_DEPRECATION_AMD,
            OpenGLExt::GL_DEBUG_CATEGORY_UNDEFINED_BEHAVIOR_AMD,
            OpenGLExt::GL_DEBUG_CATEGORY_PERFORMANCE_AMD,
            OpenGLExt::GL_DEBUG_CATEGORY_SHADER_COMPILER_AMD,
            OpenGLExt::GL_DEBUG_CATEGORY_APPLICATION_AMD,
            OpenGLExt::GL_DEBUG_CATEGORY_OTHER_AMD
         };

         for (unsigned int i = 0; i < sizeof(pDebugCat) / sizeof(*pDebugCat); ++i)
         {
            OpenGLExt::glDebugMessageEnableAMD(*(pDebugCat + i), OpenGLExt::GL_DEBUG_SEVERITY_HIGH_AMD, 0, NULL, GL_TRUE);
            OpenGLExt::glDebugMessageEnableAMD(*(pDebugCat + i), OpenGLExt::GL_DEBUG_SEVERITY_MEDIUM_AMD, 0, NULL, GL_TRUE);
            OpenGLExt::glDebugMessageEnableAMD(*(pDebugCat + i), OpenGLExt::GL_DEBUG_SEVERITY_LOW_AMD, 0, NULL, GL_TRUE);
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
      if (OpenGLExt::IsExtensionSupported("GL_ARB_debug_output"))
      {
         WGL_ASSERT(!"OpenGLWindow::DetachFromDebugContext - Needs implementation"); 
      }
      else if (OpenGLExt::IsExtensionSupported("GL_AMD_debug_output"))
      {
         // amd extension is supported here...
         // disable the callback by setting null to both params
         OpenGLExt::glDebugMessageCallbackAMD(NULL, NULL);

         // disable all errors at the highest settings...
         unsigned int pDebugCat[] =
         {
            OpenGLExt::GL_DEBUG_CATEGORY_API_ERROR_AMD,
            OpenGLExt::GL_DEBUG_CATEGORY_WINDOW_SYSTEM_AMD,
            OpenGLExt::GL_DEBUG_CATEGORY_DEPRECATION_AMD,
            OpenGLExt::GL_DEBUG_CATEGORY_UNDEFINED_BEHAVIOR_AMD,
            OpenGLExt::GL_DEBUG_CATEGORY_PERFORMANCE_AMD,
            OpenGLExt::GL_DEBUG_CATEGORY_SHADER_COMPILER_AMD,
            OpenGLExt::GL_DEBUG_CATEGORY_APPLICATION_AMD,
            OpenGLExt::GL_DEBUG_CATEGORY_OTHER_AMD
         };

         for (unsigned int i = 0; i < sizeof(pDebugCat) / sizeof(*pDebugCat); ++i)
         {
            OpenGLExt::glDebugMessageEnableAMD(*(pDebugCat + i), OpenGLExt::GL_DEBUG_SEVERITY_HIGH_AMD, 0, NULL, GL_FALSE);
            OpenGLExt::glDebugMessageEnableAMD(*(pDebugCat + i), OpenGLExt::GL_DEBUG_SEVERITY_MEDIUM_AMD, 0, NULL, GL_FALSE);
            OpenGLExt::glDebugMessageEnableAMD(*(pDebugCat + i), OpenGLExt::GL_DEBUG_SEVERITY_LOW_AMD, 0, NULL, GL_FALSE);
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
   case OpenGLExt::GL_DEBUG_CATEGORY_API_ERROR_AMD:            ssMsg << "API"; break;
   case OpenGLExt::GL_DEBUG_CATEGORY_WINDOW_SYSTEM_AMD:        ssMsg << "Window"; break;
   case OpenGLExt::GL_DEBUG_CATEGORY_DEPRECATION_AMD:          ssMsg << "Depricated"; break;
   case OpenGLExt::GL_DEBUG_CATEGORY_UNDEFINED_BEHAVIOR_AMD:   ssMsg << "Undef Behavior"; break;
   case OpenGLExt::GL_DEBUG_CATEGORY_PERFORMANCE_AMD:          ssMsg << "Performance"; break;
   case OpenGLExt::GL_DEBUG_CATEGORY_SHADER_COMPILER_AMD:      ssMsg << "Shader Compiler"; break;
   case OpenGLExt::GL_DEBUG_CATEGORY_APPLICATION_AMD:          ssMsg << "App"; break;
   case OpenGLExt::GL_DEBUG_CATEGORY_OTHER_AMD:                ssMsg << "Other"; break;
   }

   ssMsg << " - SEVERITY: ";

   switch (severity)
   {
   case OpenGLExt::GL_DEBUG_SEVERITY_HIGH_AMD:     ssMsg << "HIGH"; break;
   case OpenGLExt::GL_DEBUG_SEVERITY_MEDIUM_AMD:   ssMsg << "MEDIUM"; break;
   case OpenGLExt::GL_DEBUG_SEVERITY_LOW_AMD:      ssMsg << "LOW"; break;
   }

   ssMsg << std::endl << "MSG: " << message << std::endl;

   // output to the debug out stream
   OutputDebugString(ssMsg.str().c_str());
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