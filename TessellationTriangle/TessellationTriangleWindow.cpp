// local includes
#include "TessellationTriangleWindow.h"
//#include "Matrix.h"
//#include "Vector3.h"
//#include "WglAssert.h"
//#include "MatrixHelper.h"
//#include "OpenGLExtensions.h"

// std includes
//#include <vector>
//#include <cstdlib>
//#include <cstdint>
//#include <iostream>

TessellationTriangleWindow::TessellationTriangleWindow( ) :
mTriProgID        ( 0 ),
mTriVertArray     ( false ),
mTriVertBuffer    ( GL_ARRAY_BUFFER )
{
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
      { 0 }
   };

   // call base class to init
   if (OpenGLWindow::Create(nWidth, nHeight, pWndTitle, glInit))
   {
      // make the context current
      MakeCurrent();

      // construct the scene
      InitShaders();
      InitVertices();
  
      return true;
   }
   else
   {
      // post the quit message
      PostQuitMessage(-1);
   }

   return false;
}

// hack begin
const char * const pVertSrc =
   "#version 410\n" \
   "uniform mat4 mvp;\n" \
   "layout (location = 0) in vec3 vert_position;\n" \
   "void main( )\n" \
   "{\n" \
   "gl_Position = mvp * vec4(vert_position, 1.0f);\n" \
   "}";
const char * const pFragSrc =
   "#version 410\n" \
   "layout (location = 0) out vec4 FragColor;\n" \
   "void main( )\n" \
   "{\n" \
   "FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n" \
   "}";
#include "Matrix.h"
#include "Shaders.h"
// hack end

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
         // hack job begin
         mTriVertArray.Bind();
         glUseProgram(mTriProgID);
         Matrixf mvp = Matrixf::Ortho(-2.0f, 2, -2, 2, -100, 100) * Matrixf::LookAt(0.0f, 0, -10, 0, 0, 0, 0, 1, 0);
         GLint loc = glGetUniformLocation(mTriProgID, "mvp");
         glUniformMatrix4fv(loc, 1, GL_FALSE, mvp);
         glDrawArrays(GL_LINE_LOOP, 0, 3);
         glUseProgram(0);
         mTriVertArray.Unbind();
         SwapBuffers(GetHDC());
         // hack job end
      }
   }

   return appQuitVal;
}

LRESULT TessellationTriangleWindow::MessageHandler( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
   LRESULT result = 0;

   switch (uMsg)
   {
   default:
      // default handle the messages
      result = OpenGLWindow::MessageHandler(uMsg, wParam, lParam);
   }

   return result;
}

void TessellationTriangleWindow::InitShaders( )
{

   // hack job begin
   
   mTriProgID = glCreateProgram();
   GLuint vert = shader::LoadShaderSrc(GL_VERTEX_SHADER, pVertSrc);
   GLuint frag = shader::LoadShaderSrc(GL_FRAGMENT_SHADER, pFragSrc);
   shader::LinkShaders(mTriProgID, vert, 0, frag);

   // hack job end

}

void TessellationTriangleWindow::InitVertices( )
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
   mTriVertBuffer.GenBuffer();
   mTriVertBuffer.Bind();
   mTriVertBuffer.BufferData(sizeof(vertices), vertices, GL_STATIC_DRAW);
   mTriVertBuffer.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

   // enable vertex array index 0
   mTriVertArray.EnableVertexAttribArray(0);

   // unbind data objects
   mTriVertBuffer.Unbind();
   mTriVertArray.Unbind();
}
