// local includes
#include "RenderTargetWindow.h"
#include "WglAssert.h"
#include "OpenGLExtensions.h"

// gl includes
#include <gl/GL.h>

RenderTargetWindow::RenderTargetWindow( )
{
}

RenderTargetWindow::~RenderTargetWindow( )
{
}

bool RenderTargetWindow::Create( unsigned int nWidth,
                                 unsigned int nHeight,
                                 const char * pWndTitle,
                                 const void * pInitParams )
{
   // initialize with a 4.0 context
   const OpenGLWindow::OpenGLInit glInit40 =
   {
      4, 0, true, true, false
   };

   // initialize with a 3.2 context
   const OpenGLWindow::OpenGLInit glInit32 =
   {
      3, 3, true, false, false
   };

   // initialize 40 first, then 32 second, else nothing
   const OpenGLWindow::OpenGLInit * glInit[] =
   {
      &glInit40, &glInit32, NULL
   };

   // call base class to init
   if (OpenGLWindow::Create(nWidth, nHeight, pWndTitle, reinterpret_cast< const void ** >(glInit)))
   {
      // make the context current
      MakeCurrent();

      // attach to the debug context
      AttachToDebugContext();

      // setup the scene
      SetupScene();
      
      return true;
   }
   else
   {
      // post the quit message
      PostQuitMessage(-1);
   }

   return false;
}

int RenderTargetWindow::Run( )
{
   // app quit variables
   int appQuitVal = 0;
   bool bQuit = false;

   while (!bQuit)
   {
      // process all the app messages and then render the scene
      if (!(bQuit = PeekAppMessages(appQuitVal)))
      {
         RenderScene();
      }
   }

   return appQuitVal;
}

LRESULT RenderTargetWindow::MessageHandler( UINT uMsg,
                                            WPARAM wParam,
                                            LPARAM lParam )
{
   // default handle the messages
   return OpenGLWindow::MessageHandler(uMsg, wParam, lParam);
}

void RenderTargetWindow::SetupScene( )
{
   // set the clear color to white
   glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

   // setup the viewport
   const Size size = GetSize();
   glViewport(0, 0, size.width, size.height);

   // setup the basic projection parameters
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0.0, size.width, 0.0, size.height, -1.0, 1.0);

   // reset the modelview matrix
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   // create the render target
   CreateRenderTarget();
}

void RenderTargetWindow::CreateRenderTarget( )
{
   // set the texture width and height
   mClrAttachTexWH[0] = 1024;
   mClrAttachTexWH[1] = 32;

   // generate a texture to render into
   glGenTextures(1, &mClrAttachTex);
   glBindTexture(GL_TEXTURE_2D, mClrAttachTex);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, mClrAttachTexWH[0], mClrAttachTexWH[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
   glBindTexture(GL_TEXTURE_2D, 0);
   // attach the color texture to the framebuffer
   OpenGLExt::glGenFramebuffers(1, &mFrameBuffer);
   OpenGLExt::glBindFramebuffer(OpenGLExt::GL_DRAW_FRAMEBUFFER, mFrameBuffer);
   OpenGLExt::glFramebufferTexture2D(OpenGLExt::GL_DRAW_FRAMEBUFFER,
                                     OpenGLExt::GL_COLOR_ATTACHMENT0,
                                     GL_TEXTURE_2D, mClrAttachTex, 0);

   // explicitly turn off drawing to the depth buffer
   glDepthMask(GL_FALSE);
   glDisable(GL_DEPTH_TEST);
   
   // check that the framebuffer is complete
   const unsigned int frameBufStatus =
      OpenGLExt::glCheckFramebufferStatus(OpenGLExt::GL_DRAW_FRAMEBUFFER);

   // validate the frame buffer status
   WGL_ASSERT(frameBufStatus == OpenGLExt::GL_FRAMEBUFFER_COMPLETE);

   // clear the texture and framebuffer
   
   OpenGLExt::glBindFramebuffer(OpenGLExt::GL_DRAW_FRAMEBUFFER, 0);
}

void RenderTargetWindow::RenderScene( )
{
   // bind the frame buffer object
   OpenGLExt::glBindFramebuffer(OpenGLExt::GL_DRAW_FRAMEBUFFER, mFrameBuffer);

   // setup the viewport
   glViewport(0, 0, mClrAttachTexWH[0], mClrAttachTexWH[1]);

   // setup the basic projection parameters
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0.0, mClrAttachTexWH[0], 0.0, mClrAttachTexWH[1], -1.0, 1.0);

   // reset the modelview matrix
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   //glClear(GL_COLOR_BUFFER_BIT);
   glColor3f(0,0,1);
   glBegin(GL_QUADS);
   glVertex3d(0, 0, 0);
   glVertex3d(5000, 0, 0);
   glVertex3d(5000, 5000, 0);
   glVertex3d(0, 5000, 0);
   glEnd();

   OpenGLExt::glBindFramebuffer(OpenGLExt::GL_DRAW_FRAMEBUFFER, 0);

   glClear(GL_COLOR_BUFFER_BIT);

   // setup the viewport
   const Size size = GetSize();
   glViewport(0, 0, size.width, size.height);

   // setup the basic projection parameters
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0.0, size.width, 0.0, size.height, -1.0, 1.0);

   // reset the modelview matrix
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   glColor3f(1,0,0);
   glBegin(GL_QUADS);
   glVertex3d(0, 0, 0);
   glVertex3d(size.width, 0, 0);
   glVertex3d(size.width, size.height, 0);
   glVertex3d(0, size.height, 0);
   glEnd();

   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, mClrAttachTex);

   glBegin(GL_QUADS);
   glVertex3d(0, 0, 0); glTexCoord2d(0, 0);
   glVertex3d(100, 0, 0); glTexCoord2d(1, 0);
   glVertex3d(100, 100, 0); glTexCoord2d(1, 1);
   glVertex3d(0, 100, 0); glTexCoord2d(0, 1);
   glEnd();

   glBindTexture(GL_TEXTURE_2D, 0);
   glDisable(GL_TEXTURE_2D);

   int err = glGetError();

   SwapBuffers(GetHDC());
}