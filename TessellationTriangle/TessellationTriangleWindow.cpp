// local includes
#include "TessellationTriangleWindow.h"
#include "Matrix.h"
#include "WglAssert.h"

// std includes
#include <iostream>
#include <algorithm>

template < typename T, size_t N >
char (& array_size_t( const T (&)[N]) )[N];

#define array_size( array ) sizeof(array_size_t((array)))

TessellationTriangleWindow::TessellationTriangleWindow( ) :
mTriYaw              ( 0.0f ),
mInnerTessDivides    ( 1.0f )
{
   std::for_each(mOuterTessDivides,
                 mOuterTessDivides + array_size(mOuterTessDivides),
                 [ ] ( float & outer ) { outer = 1.0f; });
}

TessellationTriangleWindow::~TessellationTriangleWindow( )
{
}

bool TessellationTriangleWindow::Create( unsigned int nWidth,
                                         unsigned int nHeight,
                                         const char * pWndTitle,
                                         const void * pInitParams )
{
   // indicate the contexts to initialize with
   const OpenGLWindow::OpenGLInit glInit[] =
   {
      { 4, 4, false, true, false },
      { 4, 4, false, false, false },
      { 4, 3, false, true, false },
      { 4, 3, false, false, false },
      { 4, 2, false, true, false },
      { 4, 2, false, false, false },
      { 4, 1, false, true, false },
      { 4, 1, false, false, false },
      { 0 }
   };

   // call base class to init
   if (OpenGLWindow::Create(nWidth, nHeight, pWndTitle, glInit))
   {
      // make the context current
      MakeCurrent();

      // construct the scene
      const bool shaders_init = InitShaders();
      const bool vertices_init = InitVertices();

      // provide some information to the user if there is an error
      if (!shaders_init || !vertices_init)
      {
         WglMsgBox("Resource Error", !shaders_init ? "Unable to find external shader files!" : "Unable to allocate vertex buffer!");
      }
  
      return shaders_init && vertices_init;
   }
   else
   {
      // unable to initialize the gl context
      WglMsgBox("OpenGL Error", "Unable to allocate 4.1 GL context!  Application shutdown!");

      // post the quit message
      PostQuitMessage(-1);
   }

   return false;
}

int TessellationTriangleWindow::Run( )
{
   // app quit variables
   int appQuitVal = 0;
   bool bQuit = false;

   while (!bQuit)
   {
      // process all the app messages and then render the scene
      if (!(bQuit = PeekAppMessages(appQuitVal)))
      {
         // clear the back buffer
         glClear(GL_COLOR_BUFFER_BIT);

         // enable line mode to see the tess on the triangle
         glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

         // enable the shader
         mTriShaderProg.Enable();

         // enable the buffer to visualize
         mTriVertArray.Bind();

         // create a projection matrix combined with the rotation matrix
         const Matrixf mvp = Matrixf::Ortho(-1.2f, 1.2f, -1.2f, 1.2f, -100.0f, 100.0f) *
                             Matrixf::LookAt(0.0f, 0.0f, -10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f) *
                             Matrixf::Rotate(mTriYaw, 0.0f, 1.0f, 0.0f);

         // load the uniform variables in the program
         // todo: change names
         mTriShaderProg.SetUniformValue("inner", mInnerTessDivides);
         mTriShaderProg.SetUniformValue("outer", mOuterTessDivides);
         mTriShaderProg.SetUniformMatrix< 1, 4, 4 >("mvp", mvp);

         // begin drawing the triangle
         glDrawArrays(GL_PATCHES, 0, 3);

         // hack until i figure out sep shader programs
         ShaderProgram blah;
         blah.AttachFile(GL_VERTEX_SHADER, "triangle.vert");
         blah.AttachFile(GL_FRAGMENT_SHADER, "triangle.frag");
         blah.Link();
         blah.Enable();
         blah.SetUniformMatrix< 1, 4, 4 >("mvp", mvp);
         glDrawArrays(GL_LINE_LOOP, 0, 3);
         // end hack

         // disable the buffer to visualize
         mTriVertArray.Unbind();

         // disable the shader
         mTriShaderProg.Disable();

         // disable line mode
         glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

         // present the new view
         SwapBuffers(GetHDC());
      }
   }

   return appQuitVal;
}

LRESULT TessellationTriangleWindow::MessageHandler( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
   LRESULT result = 0;

   switch (uMsg)
   {
   case WM_CHAR:

      switch (wParam)
      {
      case '1':
      case '2':
         mOuterTessDivides[0] = std::max(1.0f, mOuterTessDivides[0] + (wParam == '2' ? 0.5f : -0.5f));

         break;

      case '3':
      case '4':
         mOuterTessDivides[1] = std::max(1.0f, mOuterTessDivides[1] + (wParam == '4' ? 0.5f : -0.5f));

         break;

      case '5':
      case '6':
         mOuterTessDivides[2] = std::max(1.0f, mOuterTessDivides[2] + (wParam == '6' ? 0.5f : -0.5f));

         break;

      case '7':
      case '8':
         mInnerTessDivides = std::max(1.0f, mInnerTessDivides + (wParam == '8' ? 0.5f : -0.5f));

         break;

      case 'q':
      case 'w':
         mTriYaw += wParam == 'q' ? 0.5f : -0.5f;

         break;

      case 'a':
      case 's':
         mTriYaw += wParam == 'a' ? 1.0f : -1.0f;

         break;

      case 'z':
      case 'x':
         mTriYaw += wParam == 'z' ? 3.0f : -3.0f;

         break;
      }

      {
      // update text on the window
      std::stringstream txt;
      txt << "Bottom: " << mOuterTessDivides[0]
          << " - Left: " << mOuterTessDivides[1]
          << " - Right: " << mOuterTessDivides[2]
          << " - Inner: " << mInnerTessDivides
          << std::ends;
      SetWindowText(GetHWND(), txt.str().c_str());
      }

   default:
      // default handle the messages
      result = OpenGLWindow::MessageHandler(uMsg, wParam, lParam);
   }

   return result;
}

bool TessellationTriangleWindow::InitShaders( )
{
   mTriShaderProg.AttachFile(GL_VERTEX_SHADER, "triangle.vert");
   mTriShaderProg.AttachFile(GL_FRAGMENT_SHADER, "triangle.frag");
   mTriShaderProg.AttachFile(GL_TESS_CONTROL_SHADER, "triangle.tctrl");
   mTriShaderProg.AttachFile(GL_TESS_EVALUATION_SHADER, "triangle.teval");

   return mTriShaderProg.Link();
}

bool TessellationTriangleWindow::InitVertices( )
{
   const float vertices[][3] =
   {
      {  0.0f,     1.0f, 0.0f },
      { -1.1547f, -1.0f, 0.0f },
      {  1.1547f, -1.0f, 0.0f }
   };

   // generate the vertex array
   mTriVertArray.GenArray();
   mTriVertArray.Bind();

   // create, fill, and define vertex buffer data
   mTriVertBuffer.GenBuffer(GL_ARRAY_BUFFER);
   mTriVertBuffer.Bind();
   mTriVertBuffer.BufferData(sizeof(vertices), vertices, GL_STATIC_DRAW);
   mTriVertBuffer.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(*vertices), 0);

   // enable vertex array index 0
   mTriVertArray.EnableVertexAttribArray(0);

   // unbind data objects
   mTriVertBuffer.Unbind();
   mTriVertArray.Unbind();

   return mTriVertArray && mTriVertBuffer;
}
