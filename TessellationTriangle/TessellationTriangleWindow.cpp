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
   "layout (location = 0) in vec3 vert_position;\n" \
   "uniform mat4 mvp;\n" \
   "out vec4 vert_world_pos_cs;\n" \
   "void main( )\n" \
   "{\n" \
   //"vert_world_pos_cs = vec4(vert_position, 1.0f);\n" \

   "gl_Position = mvp * vec4(vert_position, 1.0f);\n" \
   "}";
const char * const pFragSrc =
   "#version 410\n" \
   "layout (location = 0) out vec4 FragColor;\n" \
   "void main( )\n" \
   "{\n" \
   "FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n" \
   "}";
const char * const pTCtrlSrc =
   "#version 410\n" \
   "layout (vertices = 3) out;\n" \
   "in vec4 vert_world_pos_cs[];\n" \
   "out vec4 vert_world_pos_es[];\n" \
   "uniform vec3 outer;\n" \
   "uniform float inner;\n" \
   "void main( )\n" \
   "{\n" \
   "gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;\n" \
   "if (gl_InvocationID==0)\n" \
   "{\n" \
   "gl_TessLevelOuter[0] = outer.x;\n" \
   "gl_TessLevelOuter[1] = outer.y;\n" \
   "gl_TessLevelOuter[2] = outer.z;\n" \
   "gl_TessLevelInner[0] = inner;\n" \
   "}\n" \
   "}\n";
const char * const pTEvalSrc =
   "#version 410\n" \
   "uniform mat4 mvp;\n" \
   "layout (triangles, equal_spacing, ccw) in;\n" \
   "in vec4 vert_world_pos_es[];\n" \
   "void main( )\n" \
   "{\n" \
   //"gl_Position = mvp * vec4((gl_TessCoord.x * vert_world_pos_es[0].xyz), 1.0f);\n" \

   "gl_Position = mvp * gl_in[0].gl_Position * gl_TessCoord.x + gl_in[1].gl_Position * gl_TessCoord.y + gl_in[2].gl_Position * gl_TessCoord.z;\n" \
   "}\n";
#include "Matrix.h"
#include "Shaders.h"
#include "Timer.h"
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
         glClear(GL_COLOR_BUFFER_BIT);
         glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
         mTriVertArray.Bind();
         //glUseProgram(mTriProgID);
         mTriShaderProg.Enable();
         static float rot = 0.0f; rot += 0.01f;
         Matrixf mvp = Matrixf::Ortho(-1.2f, 1.2f, -1.2, 1.2, -100, 100) * Matrixf::LookAt(0.0f, 0, -10, 0, 0, 0, 0, 1, 0) * Matrixf::Rotate(rot, 0, 1, 0);
         GLint loc = mTriShaderProg.GetUniformLocation("mvp");
         GLint inner = mTriShaderProg.GetUniformLocation("inner");
         float outer_values[] = { std::abs(std::sin(Timer().GetCurrentTimeSec())) * 50, 
                                std::abs(std::sin(Timer().GetCurrentTimeSec())) * 50,
                                std::abs(std::sin(Timer().GetCurrentTimeSec())) * 50};
         GLint outer = mTriShaderProg.GetUniformLocation("outer");
         float inner_value = (std::abs(std::sin(Timer().GetCurrentTimeSec()))) * 50;
         //glUniform3fv(outer, 1, outer_values);
         mTriShaderProg.SetUniformValue("outer", outer_values);
         //glUniform1f(inner, inner_value);
         mTriShaderProg.SetUniformValue("inner", inner_value);
         glUniformMatrix4fv(loc, 1, GL_FALSE, mvp);
         glPatchParameteri(GL_PATCH_VERTICES, 3);
         glDrawArrays(GL_PATCHES, 0, 3);
         //glUseProgram(0);
         mTriShaderProg.Disable();
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
  
   //GLuint vert = shader::LoadShaderSrc(GL_VERTEX_SHADER, pVertSrc);
   //GLuint frag = shader::LoadShaderSrc(GL_FRAGMENT_SHADER, pFragSrc);
   //GLuint tctrl = shader::LoadShaderSrc(GL_TESS_CONTROL_SHADER, pTCtrlSrc);
   //GLuint teval = shader::LoadShaderSrc(GL_TESS_EVALUATION_SHADER, pTEvalSrc);
   //shader::LinkShaders(mTriProgID, vert, 0, frag, tctrl, teval);
   mTriShaderProg.Attach(GL_VERTEX_SHADER, pVertSrc);
   mTriShaderProg.Attach(GL_FRAGMENT_SHADER, pFragSrc);
   mTriShaderProg.Attach(GL_TESS_CONTROL_SHADER, pTCtrlSrc);
   mTriShaderProg.Attach(GL_TESS_EVALUATION_SHADER, pTEvalSrc);
   mTriShaderProg.Link();

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
   mTriVertBuffer.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(*vertices), 0);

   // enable vertex array index 0
   mTriVertArray.EnableVertexAttribArray(0);

   // unbind data objects
   mTriVertBuffer.Unbind();
   mTriVertArray.Unbind();
}
