// local includes
#include "InstancingWindow.h"
#include "Timer.h"
#include "Matrix.h"
#include "Shaders.h"
#include "Vector4.h"
#include "ReadTexture.h"

// gl includes
#include "GL/glew.h"
#include <GL/GL.h>

// std includes
#include <stdint.h>

// stl includes
#include <sstream>
#include <iomanip>

// determines if the application should run wild
#define LET_APP_RUN_WILD 0

InstancingWindow::InstancingWindow( ) :
mVertArrayID      ( 0 ),
mVertexBufferID   ( 0 ),
mIndexBufferID    ( 0 ),
mIndexBufferSize  ( 0 ),
mTexBufferID      ( 0 ),
mTextureID        ( 0 ),
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
   Timer localTimer;

   // setup the amount of timer per frame
#if LET_APP_RUN_WILD
   const double MS_PER_FRAME = 0.0;
#else
   const double MS_PER_FRAME = 1000.0 / 60.0;
#endif

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

         // determine the framrate of the application
         const double frame_rate = 1.0 / localTimer.DeltaSec(begTick);

         // construct a string to place on the title
         std::stringstream ss;
         ss << "Instancing - " << std::fixed << std::setprecision(3) << frame_rate << " fps";
         SetWindowText(GetHWND(), ss.str().c_str());
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
      -1.0f, 0.0f,  1.0f,  1.0f, 0.0f,  1.0f,  1.0f, 5.0f,  1.0f, -1.0f, 5.0f,  1.0f,
       1.0f, 0.0f,  1.0f,  1.0f, 0.0f, -1.0f,  1.0f, 5.0f, -1.0f,  1.0f, 5.0f,  1.0f,
       1.0f, 0.0f, -1.0f, -1.0f, 0.0f, -1.0f, -1.0f, 5.0f, -1.0f,  1.0f, 5.0f, -1.0f,
      -1.0f, 0.0f, -1.0f, -1.0f, 0.0f,  1.0f, -1.0f, 5.0f,  1.0f, -1.0f, 5.0f, -1.0f,
      -1.0f, 5.0f,  1.0f,  1.0f, 5.0f,  1.0f,  1.0f, 5.0f, -1.0f, -1.0f, 5.0f, -1.0f
   };

   // create, fill, and define the vertex array data
   glGenBuffers(1, &mVertexBufferID);
   glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferID);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

   // define the texture coordinate values
   const float tex_left = 19.0f / 1023.0f;
   const float tex_right = 185.0f / 1023.0f;
   const float tex_bottom = (1023.0f - 239.0f) / 1023.0f;
   const float tex_top = (1023.0f - 18.0f) / 1023.0f;
   const float tex_roof_left = 24.0f / 1023.0f;
   const float tex_roof_right = 99.0f / 1023.0f;
   const float tex_roof_bottom = (1023.0f - 634.0f) / 1023.0f;
   const float tex_roof_top = (1023.0f - 558.0f) / 1023.0f;

   const float tex_coords[] =
   {
      tex_left, tex_bottom, tex_right, tex_bottom, tex_right, tex_top, tex_left, tex_top,
      tex_left, tex_bottom, tex_right, tex_bottom, tex_right, tex_top, tex_left, tex_top,
      tex_left, tex_bottom, tex_right, tex_bottom, tex_right, tex_top, tex_left, tex_top,
      tex_left, tex_bottom, tex_right, tex_bottom, tex_right, tex_top, tex_left, tex_top,
      tex_roof_left, tex_roof_bottom, tex_roof_right, tex_roof_bottom, tex_roof_right, tex_roof_top, tex_roof_left, tex_roof_top
   };

   // create, fill, and define the texture coord array data
   glGenBuffers(1, &mTexBufferID);
   glBindBuffer(GL_ARRAY_BUFFER, mTexBufferID);
   glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coords), tex_coords, GL_STATIC_DRAW);
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

   // define the instance index ids
   const uint32_t indices[] =
   {
      0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19
   };

   // create, fill, and define the index array data
   glGenBuffers(1, &mIndexBufferID);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferID);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

   // make sure to define the number of indices
   mIndexBufferSize = sizeof(indices) / sizeof(*indices);

   // done with the vertex array manipulation
   glBindVertexArray(0);
}

void InstancingWindow::RenderScene( )
{
   static float rot = 0;
   float len = 5.5f;
   float x = std::cos(rot) * len;
   float y = std::cos(rot) * 10.0f;
   float z = std::sin(rot) * len;
   rot += (15.0f / 30.0f * (M_PI / 180.0f));

   const Matrixf pvw =
      Matrixf::Perspective(45.0f, 800.0f / 600.0f, 1.0f, 50.0f) *
      Matrixf::LookAt(x, y, z, 0.0f, 2.5f, 0.0f, 0.0f, 1.0f, 0.0f) *
      Matrixf();

   static GLuint prog = 0;
   static GLuint vert = 0;
   static GLuint frag = 0;

   if (!prog)
   {
      prog = glCreateProgram();

      vert = shader::LoadShaderFile(GL_VERTEX_SHADER, "vertex.vert");
      frag = shader::LoadShaderFile(GL_FRAGMENT_SHADER, "fragment.frag");

      shader::LinkShaders(prog, vert, 0, frag);

      uint32_t width, height;
      uint8_t * pTexture = nullptr;
      ReadRGB("buildings.rgb", width, height, &pTexture);

      glGenTextures(1, &mTextureID);
      glBindTexture(GL_TEXTURE_2D, mTextureID);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pTexture);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glBindTexture(GL_TEXTURE_2D, 0);
   }

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glEnable(GL_CULL_FACE);

   glUseProgram(prog);

   glEnable(GL_TEXTURE_2D);
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, mTextureID);

   GLint mat_loc = glGetUniformLocation(prog, "gPVW");
   glUniformMatrix4fv(mat_loc, 1, GL_FALSE, pvw);

   glBindVertexArray(mVertArrayID);

   glDrawElements(GL_QUADS, mIndexBufferSize, GL_UNSIGNED_INT, nullptr);

   glBindVertexArray(0);

   glDisable(GL_TEXTURE_2D);

   glUseProgram(0);

   // swap the buffers to display
   SwapBuffers(GetHDC());
}

