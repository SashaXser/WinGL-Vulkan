// local includes
#include "InstancingWindow.h"
#include "Timer.h"
#include "Matrix.h"
#include "Shaders.h"
#include "Vector4.h"

// gl includes
#include "GL/glew.h"
#include <GL/GL.h>

// std includes
#include <stdint.h>

InstancingWindow::InstancingWindow( ) :
mVertArrayID      ( 0 ),
mVertexBufferID   ( 0 ),
mIndexBufferID    ( 0 ),
mVertShaderID     ( 0 ),
mFragShaderID     ( 0 )
{
}

InstancingWindow::~InstancingWindow( )
{
}

bool InstancingWindow::Create( unsigned int nWidth, unsigned int nHeight,
                               const char * pWndTitle, const void ** pInitParams )
{
   // initialize gl context params
   const OpenGLWindow::OpenGLInit glInit41D =
   {
      4, 1, true, true, false
   };

   const OpenGLWindow::OpenGLInit glInit41 =
   {
      4, 1, true, false, false
   };

   const OpenGLWindow::OpenGLInit glInit40D =
   {
      4, 0, true, true, false
   };

   const OpenGLWindow::OpenGLInit glInit40 =
   {
      4, 0, true, false, false
   };

   const OpenGLWindow::OpenGLInit glInit33D =
   {
      3, 3, true, true, false
   };

   const OpenGLWindow::OpenGLInit glInit33 =
   {
      3, 3, true, false, false
   };

   const OpenGLWindow::OpenGLInit * glInit[] =
   {
      &glInit41D, &glInit41, &glInit40D, &glInit40, &glInit33D, &glInit33, nullptr
   };

   if (OpenGLWindow::Create(nWidth, nHeight, pWndTitle,
                            reinterpret_cast< const void ** >(glInit)))
   {
      // make the gl context active
      MakeCurrent();

      // create all the instances
      CreateInstances();

      return true;
   }

   return false;
}

int InstancingWindow::Run( )
{
   // create a local timer object...
   // timer object will be used to create
   // a 60 Hz rendering loop...
   Timer localTimer;

   // setup the amount of timer per frame
   const double MS_PER_FRAME = 1000.0 / 60.0;

   // app quit variables
   int appQuitVal = 0;
   bool bQuit = false;

   while (!bQuit)
   {
      // obtain the current tick count
      const long long begTick = localTimer.GetCurrentTick();

      // process all the app messages and then render the scene
      if (!(bQuit = PeekAppMessages(appQuitVal)))
      {
         // render the scene
         RenderScene();

         // if there is time left, then do some waiting
         if (const double deltaMS = localTimer.DeltaMS(begTick) <= MS_PER_FRAME)
         {
            // wait for the remainder of the time
            localTimer.Wait(static_cast< unsigned long >(MS_PER_FRAME - deltaMS));
         }
      }
   }

   return appQuitVal;
}

void InstancingWindow::CreateInstances( )
{
   // generate the vertex array object
   glGenVertexArrays(1, &mVertArrayID);
   glBindVertexArray(mVertArrayID);

   // define the instance vertex positions
   const float vertices[] =
   {
      -1.0f, -1.0f, 0.0f,  1.0f, -1.0f, 0.0f, // 0 1
       1.0f,  1.0f, 0.0f, -1.0f,  1.0f, 0.0f, // 2 3
      -1.0f, -1.0f, 4.0f,  1.0f, -1.0f, 4.0f, // 4 5
       1.0f,  1.0f, 4.0f, -1.0f,  1.0f, 4.0f  // 6 7
   };

   // create, fill, and define the vertex array data
   glGenBuffers(1, &mVertexBufferID);
   glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferID);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

   // define the instance index ids
   const uint32_t indices[] =
   {
      0, 1, 5, 4, 1, 2, 6, 5,
      2, 3, 7, 6, 3, 0, 4, 7,
      5, 6, 7, 4
   };

   // create, fill, and define the index array data
   glGenBuffers(1, &mIndexBufferID);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferID);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

   // done with the vertex array manipulation
   glBindVertexArray(0);
}

void InstancingWindow::RenderScene( )
{
   const Matrixf pvw =
      Matrixf::Perspective(45.0f, 800.0f / 600.0f, 1.0f, 50.0f) *
      Matrixf::Translate(0.0f, 0.0f, -5.0f) *
      Matrixf();

   // swap the buffers to display
   SwapBuffers(GetHDC());
}

