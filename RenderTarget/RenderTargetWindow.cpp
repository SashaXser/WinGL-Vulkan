// local includes
#include "RenderTargetWindow.h"
#include "WglAssert.h"
#include "OpenGLExtensions.h"

// gl includes
#include "GL/glew.h"
#include <gl/GL.h>

RenderTargetWindow::RenderTargetWindow( ) :
MAX_WIDTH      ( 300.0 )
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
   // initialize 40 first, then 32 second, else nothing
   const OpenGLWindow::OpenGLInit glInit[] =
   {
      { 4, 0, true, true, false },
      { 3, 3, true, false, false },
      { 0 }

   };

   // call base class to init
   if (OpenGLWindow::Create(nWidth, nHeight, pWndTitle, glInit))
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
   switch (uMsg)
   {
   case WM_SIZING:
      RenderScene(); return 0;

      break;

   default:
      // default handle the messages
      return OpenGLWindow::MessageHandler(uMsg, wParam, lParam);
   }
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

   // attach the color texture to the framebuffer
   glGenFramebuffers(1, &mFrameBuffer);
   glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFrameBuffer);
   glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,
                          GL_COLOR_ATTACHMENT0,
                          GL_TEXTURE_2D, mClrAttachTex, 0);
   
   // check that the framebuffer is complete
   const unsigned int frameBufStatus = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);

   // validate the frame buffer status
   WGL_ASSERT(frameBufStatus == GL_FRAMEBUFFER_COMPLETE);

   // clear the texture and framebuffer
   glBindTexture(GL_TEXTURE_2D, 0);
   glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

   // render the texture
   RenderTexture();
}

void RenderTargetWindow::RenderTexture( )
{
   // set all the attribute bits
   glPushAttrib(GL_ALL_ATTRIB_BITS);

   // bind the frame buffer object
   glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFrameBuffer);

   // setup the viewport
   glViewport(0, 0, mClrAttachTexWH[0], mClrAttachTexWH[1]);

   // setup the basic projection parameters
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(mClrAttachTexWH[0] * -0.5, mClrAttachTexWH[0] * 0.5,
           0.0, mClrAttachTexWH[1],
           -1.0, 1.0);

   // reset the modelview matrix
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   // disable depth buffer reads and writes
   glDepthMask(GL_FALSE);
   glDisable(GL_DEPTH_TEST);

   // clear the background
   glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
   glClear(GL_COLOR_BUFFER_BIT);

   // render red box
   glBegin(GL_QUADS);
   glColor3f(1.0f, 0.0f, 0.0f);
   glVertex3f(-400.0f, -100.0f, 0.0f);
   glVertex3f( 400.0f, -100.0f, 0.0f);
   glVertex3f( 400.0f,  100.0f, 0.0f);
   glVertex3f(-400.0f,  100.0f, 0.0f);
   glEnd();

   // render blue box
   glBegin(GL_QUADS);
   glColor3f(0.0f, 0.0f, 1.0f);
   glVertex3f(-200.0f, -100.0f, 0.0f);
   glVertex3f( 200.0f, -100.0f, 0.0f);
   glVertex3f( 200.0f,  100.0f, 0.0f);
   glVertex3f(-200.0f,  100.0f, 0.0f);
   glEnd();

   // render green box
   glBegin(GL_QUADS);
   glColor3f(0.0f, 1.0f, 0.0f);
   glVertex3f(-16.0f, -100.0f, 0.0f);
   glVertex3f( 16.0f, -100.0f, 0.0f);
   glVertex3f( 16.0f,  100.0f, 0.0f);
   glVertex3f(-16.0f,  100.0f, 0.0f);
   glEnd();

   // render two white lines for the boarders
   glLineWidth(4);
   glBegin(GL_LINES);
   glColor3f(1.0f, 1.0f, 1.0f);
   glVertex3d(MAX_WIDTH * -0.5,  100.0, 0.0f);
   glVertex3d(MAX_WIDTH * -0.5, -100.0, 0.0f);
   glVertex3d(MAX_WIDTH *  0.5,  100.0, 0.0f);
   glVertex3d(MAX_WIDTH *  0.5, -100.0, 0.0f);
   glEnd();

   // unbind the framebuffer object
   glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

   // restore all the attributes
   glPopAttrib();
}

void RenderTargetWindow::RenderScene( )
{
   // clear the background
   glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
   glClear(GL_COLOR_BUFFER_BIT);

   // setup the viewport
   const Size size = GetSize();
   glViewport(0, 0, size.width, size.height);

   // setup the basic projection parameters
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(size.width * -0.5, size.width * 0.5,
           size.height * -0.5f, size.height * 0.5f,
           -1.0, 1.0);

   // reset the modelview matrix
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   // determine the extents of the quad
   double rightExt = mClrAttachTexWH[0] * 0.5;
   double leftExt = mClrAttachTexWH[0] * -0.5;
   double bottomExt = mClrAttachTexWH[1] * -0.5;
   double topExt = mClrAttachTexWH[1] * 0.5;

   if (MAX_WIDTH > size.width)
   {
      // recalculate the extents
      const double extPercentage = 1.0 - (MAX_WIDTH - size.width) / MAX_WIDTH;
      // update the left and right extents of the quad
      rightExt *= extPercentage;
      leftExt *= extPercentage;
      // determine the new height size
      const double height =
         static_cast< double >(mClrAttachTexWH[1]) /
         static_cast< double >(mClrAttachTexWH[0]) *
         (rightExt * 2.0);
      // update the bottom and top extents of the quad
      topExt = height * 0.5;
      bottomExt = height * -0.5;
   }

   // enable texturing
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, mClrAttachTex);

   // render a quad the size of the texture
   glBegin(GL_QUADS);
   glColor3f(1.0f, 1.0f, 1.0f);
   glTexCoord2f(0.0f, 0.0f); glVertex3d(leftExt, bottomExt, 0.0);
   glTexCoord2f(1.0f, 0.0f); glVertex3d(rightExt, bottomExt, 0.0);
   glTexCoord2f(1.0f, 1.0f); glVertex3d(rightExt, topExt, 0.0);
   glTexCoord2f(0.0f, 1.0f); glVertex3d(leftExt, topExt, 0.0);
   glEnd();

   // disable texturing
   glDisable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, 0);

   // swap the front and back buffers
   SwapBuffers(GetHDC());
}