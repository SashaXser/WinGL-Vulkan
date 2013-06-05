// local includes
#include "ShadowMapWindow.h"
#include "WglAssert.h"
#include "OpenGLExtensions.h"
#include "Matrix.h"
#include "Vector3.h"

ShadowMapWindow::ShadowMapWindow( )
{
   memset(&mFloor, 0x00, sizeof(mFloor));
   memset(&mCube, 0x00, sizeof(mCube));
   memset(&mSphere, 0x00, sizeof(mSphere));
   memset(&mPyramid, 0x00, sizeof(mPyramid));
}

ShadowMapWindow::~ShadowMapWindow( )
{
}

bool ShadowMapWindow::Create( unsigned int nWidth,
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
   if (OpenGLWindow::Create(nWidth, nHeight, pWndTitle,
                            reinterpret_cast< const void ** >(glInit)))
   {
      // make the context current
      MakeCurrent();

      // attach to the debug context
      AttachToDebugContext();

      // create the specific data
      GenerateSceneData();

      // enable specific state
      glEnable(GL_CULL_FACE);
      
      return true;
   }
   else
   {
      // post the quit message
      PostQuitMessage(-1);
   }

   return false;
}

int ShadowMapWindow::Run( )
{
   // app quit variables
   int appQuitVal = 0;
   bool bQuit = false;

   while (!bQuit)
   {
      // process all the app messages and then render the scene
      if (!(bQuit = PeekAppMessages(appQuitVal)))
      {
         // render the scene
         RenderScene();
      }
   }

   return appQuitVal;
}

LRESULT ShadowMapWindow::MessageHandler( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
   LRESULT result = 0;

   switch (uMsg)
   {
   case WM_SIZE:
      // update the viewport
      glViewport(0, 0, lParam & 0xFFFF, lParam >> 16);

      break;

   default:
      // default handle the messages
      result = OpenGLWindow::MessageHandler(uMsg, wParam, lParam);
   }

   return result;
}

void ShadowMapWindow::RenderScene( )
{
   // clear the buffers
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   

   // temp
   Matrixf proj = Matrixf::Perspective(45.0, 4./3., 0.1, 3000.);
   Matrixf mv = Matrixf::LookAt(100, 20, -100, -100.0f, 0, 100.0f, 0, 1.0f, 0);
   static float rot = 0;
   rot += 0.05f;
   mv *= Matrixf::Rotate(rot, Vec3f(0,1,0));
   glMatrixMode(GL_PROJECTION);
   glLoadMatrixf(proj);
   glMatrixMode(GL_MODELVIEW);
   glLoadMatrixf(mv);
   glBindVertexArray(mFloor.mVAO);
   glColor3f(1,1,1);
   glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, nullptr);
   glBindVertexArray(mCube.mVAO);
   glColor3f(1,0,0);
   glDrawElements(GL_QUADS, 24, GL_UNSIGNED_INT, nullptr);
   glBindVertexArray(0);

 


   // swap the front and back
   SwapBuffers(GetHDC());
}

void ShadowMapWindow::GenerateSceneData( )
{
   GenerateFloor();
   GenerateCube();
   GenerateSphere();
   GeneratePyramid();
}

void ShadowMapWindow::GenerateFloor( )
{
   // create the vao
   glGenVertexArrays(1, &mFloor.mVAO);
   glBindVertexArray(mFloor.mVAO);

   // create some very basic vertex data
   const float verts[] =
   {
      -100.0f, 0.0f,  100.0f,  100.0f, 0.0f,  100.0f,
       100.0f, 0.0f, -100.0f, -100.0f, 0.0f, -100.0f
   };

   // create and fill the vertex buffer
   glGenBuffers(1, &mFloor.mVertBufID);
   glBindBuffer(GL_ARRAY_BUFFER, mFloor.mVertBufID);
   glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
   glEnableVertexAttribArray(0);

   // create the index vertex data
   const GLuint indices[] = { 0, 1, 2, 3 };

   // crate and fill the index buffer
   glGenBuffers(1, &mFloor.mIdxBufID);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mFloor.mIdxBufID);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

   // disable the vao
   glBindVertexArray(0);
}

void ShadowMapWindow::GenerateCube( )
{
   // create the vao
   glGenVertexArrays(1, &mCube.mVAO);
   glBindVertexArray(mCube.mVAO);

   // create some very basic vertex data
   const float verts[] =
   {
      -10.0f, -10.0f,  10.0f,  10.0f, -10.0f,  10.0f,  10.0f,  10.0f,  10.0f, -10.0f,  10.0f,  10.0f,
       10.0f, -10.0f,  10.0f,  10.0f, -10.0f, -10.0f,  10.0f,  10.0f, -10.0f,  10.0f,  10.0f,  10.0f,
       10.0f, -10.0f, -10.0f, -10.0f, -10.0f, -10.0f, -10.0f,  10.0f, -10.0f,  10.0f,  10.0f, -10.0f,
      -10.0f, -10.0f, -10.0f, -10.0f, -10.0f,  10.0f, -10.0f,  10.0f,  10.0f, -10.0f,  10.0f, -10.0f,
      -10.0f, -10.0f, -10.0f,  10.0f, -10.0f, -10.0f,  10.0f, -10.0f,  10.0f, -10.0f, -10.0f,  10.0f,
      -10.0f,  10.0f,  10.0f,  10.0f,  10.0f,  10.0f,  10.0f,  10.0f, -10.0f, -10.0f,  10.0f, -10.0f
   };

   // create and fill the vertex buffer
   glGenBuffers(1, &mCube.mVertBufID);
   glBindBuffer(GL_ARRAY_BUFFER, mCube.mVertBufID);
   glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
   glEnableVertexAttribArray(0);

   // create the index vertex data
   const GLuint indices[] =
   {
       0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
      10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
      20, 21, 22, 23
   };

   // crate and fill the index buffer
   glGenBuffers(1, &mCube.mIdxBufID);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mCube.mIdxBufID);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

   // disable the vao
   glBindVertexArray(0);
}

void ShadowMapWindow::GenerateSphere( )
{
}

void ShadowMapWindow::GeneratePyramid( )
{
}

