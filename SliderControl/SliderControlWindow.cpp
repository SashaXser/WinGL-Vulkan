// local includes
#include "SliderControlWindow.h"

// wgl includes
#include "Timer.h"
#include "Matrix.h"
#include "Vector.h"
#include "MathHelper.h"

// gl includes
#include <GL/glew.h>
#include <GL/GL.h>

// std incluces
#include <vector>
#include <cmath>
//#include <cstdint>
//#include <iostream>

SliderControlWindow::SliderControlWindow( ) :
mHandlePos        ( 0.0f ),
mHandleRot        ( 0.0f ),
mCurrentTimeTick  ( Timer().GetCurrentTick() )
{
}

SliderControlWindow::~SliderControlWindow( )
{
   
}

bool SliderControlWindow::Create( unsigned int nWidth,
                                  unsigned int nHeight,
                                  const char * pWndTitle,
                                  const void * pInitParams )
{
   // initialize with a context else nothing
   const OpenGLWindow::OpenGLInit glInit[] =
   {
      { 4, 1, false, true, false },
      { 0 }
   };

   // call base class to init
   if (OpenGLWindow::Create(nWidth, nHeight, pWndTitle, glInit))
   {
      // make the context current
      MakeCurrent();

      // intialize the camera
      mCamera.LookAt({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f });

      // enable the depth buffer and cull faces
      mPipeline.EnableCullFace(true);
      mPipeline.EnableDepthTesting(true);

      // init all things gl
      if (InitGLData())
      {
         // force the projection matrix to get calculated and updated
         SendMessage(GetHWND(), WM_SIZE, 0, nHeight << 16 | nWidth);

         return true;
      }
   }

   return false;
}

void SliderControlWindow::OnDestroy( )
{
   // call the base class destroy
   OpenGLWindow::OnDestroy();
}
            
int SliderControlWindow::Run( )
{
   // vars for the loop
   int appQuitVal = 0;
   bool bQuit = false;

   // basic message pump and render frame
   while (!bQuit)
   {
      // process all the messages
      if (!(bQuit = PeekAppMessages(appQuitVal)))
      {
         // clear the back buffer and the depth buffer
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

         // prepare the line for rendering
         mGrooveVAO.Bind();
         mGrooveShader.Enable();

         // setup the model view projection matrix
         mGrooveShader.SetUniformMatrix< 1, 4, 4 >("mvp", mCamera.GetProjectionMatrix() * mCamera.GetViewMatrix());

         // temp
         //const Vec3f groove_vertices[] = { Vec3f(0.0f, -1.0f, 0.0f), Vec3f(0.0f, 1.0f, 0.0f) };
         //mGrooveShader.SetUniformValue< 3 >("vertices",
         //                                   static_cast< const float * >(*groove_vertices),
         //                                   sizeof(groove_vertices) / sizeof(*groove_vertices));
         mGrooveShader.SetUniformValue("line_color", 1.0f, 0.0f, 0.0f);
         mGrooveShader.SetUniformValue("widget_width", static_cast< float >(GetSize().width));
         // end temp

         // render the groove shape
         mPipeline.DrawArrays(GL_TRIANGLES, 0, 6);

         // unprepare the line from rendering         
         mGrooveShader.Disable();
         mGrooveVAO.Unbind();

         // prepare the handle for rendering
         mHandleVAO.Bind();
         mHandleShader.Enable();

         // setup the texture for rendering
         mYokeTex.Bind(GL_TEXTURE0);
         mHandleShader.SetUniformValue("handle_texture", static_cast< GLint >(mYokeTex.GetBoundSamplerID()));

         // how much time has passed since the last call
         const double time_slice_sec = Timer().DeltaSec(mCurrentTimeTick);
         mHandlePos += math::pi< float >() * time_slice_sec / 2.85f;
         mHandleRot += math::pi< float >() * time_slice_sec;

         // setup the model view projection matrix
         mHandleShader.SetUniformMatrix< 1, 4, 4 >("mvp",
                                                   mCamera.GetProjectionMatrix() * mCamera.GetViewMatrix() *
                                                   Matrixf::Translate(0.0f, std::sin(mHandlePos), 0.5f) *
                                                   Matrixf::Rotate(std::cos(mHandleRot) * 135.0f, 0.0f, 0.0f, 1.0f));

         // enable alpha blending
         mPipeline.EnableStandardBlending(true);

         // render the groove shape
         mPipeline.DrawArrays(GL_TRIANGLES, 0, 6);

         // disable alpha blending
         mPipeline.EnableStandardBlending(false);

         // unprepare the handle for rendering
         mHandleShader.Disable();
         mHandleVAO.Unbind();

         // swap the front and back buffers
         SwapBuffers(GetHDC());

         // save off the current time
         mCurrentTimeTick = Timer().GetCurrentTick();
      }
   }

   return appQuitVal;
}

LRESULT SliderControlWindow::MessageHandler( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
   LRESULT result = 0;

   switch (uMsg)
   {
   case WM_SIZE:
      {
      // get the width and height
      const int32_t width = static_cast< int32_t >(lParam & 0x0000FFFF);
      const int32_t height = static_cast< int32_t >(lParam >> 16);

      // change the size of the viewport
      // this will be ignored until a valid gl context is created
      glViewport(0, 0,
                 static_cast< GLsizei >(width),
                 static_cast< GLsizei >(height));

      // update the projection matrix
      mCamera.SetOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
      }

      break;

   default:
      // allow default processing to happen
      result = OpenGLWindow::MessageHandler(uMsg, wParam, lParam);
   }
   
   return result;
}

bool SliderControlWindow::InitGLData( )
{
   // read in the texture data
   const bool yoke_loaded = mYokeTex.Load2D("generic_yoke.png", GL_RGBA, GL_COMPRESSED_RGBA, true);
   const bool pedal_loaded = mPedalTex.Load2D("generic_pedal.png", GL_RGBA, GL_COMPRESSED_RGBA, true);

   // initialize the groove vertex array object...
   // this is needed as the core requires it even if it is empty...
   mGrooveVAO.GenArray();

   // load the basic shaders
   mGrooveShader.Attach(GL_VERTEX_SHADER,
                        "#version 410 core\n"
                        "uniform mat4 mvp;\n"
                        "uniform float widget_width;\n"
                        "\n"
                        "void main( )\n"
                        "{\n"
                        "   float half_w = 2.0f / widget_width * 8;\n"
                        "   vec2 vertices[6] = vec2[6](\n"
                        "   vec2(-half_w, 1), vec2(-half_w, -1), vec2(half_w, -1),\n"
                        "   vec2(-half_w, 1), vec2(half_w, -1), vec2(half_w, 1)\n"
                        "   );\n"
                        "   gl_Position = mvp * vec4(vertices[gl_VertexID], 0.0f, 1.0f);\n"
                        "}\n");
   mGrooveShader.Attach(GL_FRAGMENT_SHADER,
                        "#version 410 core\n"
                        "layout (location = 0) out vec4 output_0;\n"
                        "uniform vec3 line_color;\n"
                        "\n"
                        "void main( )\n"
                        "{\n"
                        "   output_0 = vec4(line_color, 1.0f);\n"
                        "}\n");

   mHandleShader.Attach(GL_VERTEX_SHADER,
                        "#version 410 core\n"
                        "uniform mat4 mvp;\n"
                        "layout (location = 0) in vec3 vertices;\n"
                        "out smooth vec2 tcoords;\n"
                        "\n"
                        "void main( )\n"
                        "{\n"
                        "   gl_Position = mvp * vec4(vertices, 1.0f);\n"
                        "   const vec2 texture_coordinates[6] =\n"
                        "   {\n"
                        "      vec2(0.0f, 1.0f), vec2(0.0f, 0.0f), vec2(1.0f, 0.0f),\n"
                        "      vec2(0.0f, 1.0f), vec2(1.0f, 0.0f), vec2(1.0f, 1.0f)\n"
                        "   };\n"
                        "   tcoords = texture_coordinates[gl_VertexID];\n"
                        "}\n");
   mHandleShader.Attach(GL_FRAGMENT_SHADER,
                        "#version 410 core\n"
                        "uniform sampler2D handle_texture;\n"
                        "layout (location = 0) out vec4 output_0;\n"
                        "in smooth vec2 tcoords;\n"
                        "\n"
                        "void main( )\n"
                        "{\n"
                        "   output_0 = texture(handle_texture, tcoords);\n"
                        "}\n");

   // prepare the vertices for rendering
   mHandleVAO.GenArray();
   mHandleVAO.Bind();

   mHandleVBO.GenBuffer(GL_ARRAY_BUFFER);
   mHandleVBO.Bind();
   const std::vector< Vec3f > handle_vertices =
   {
      Vec3f(-0.1f, 0.1f, 0.0f), Vec3f(-0.1f, -0.1f, 0.0f), Vec3f(0.1f, -0.1f, 0.0f),
      Vec3f(-0.1f, 0.1f, 0.0f), Vec3f(0.1f, -0.1f, 0.0f), Vec3f(0.1f, 0.1f, 0.0f)
   };
   mHandleVBO.BufferData(handle_vertices.size() * sizeof(Vec3f), handle_vertices.data(), GL_STATIC_DRAW);
   mHandleVBO.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
   mHandleVAO.EnableVertexArrayAttrib(0);
   mHandleVAO.EnableVertexArrayAttrib(1);
   mHandleVBO.Unbind();

   mHandleVAO.Unbind();

   return yoke_loaded && pedal_loaded &&
          mGrooveVAO && mHandleVAO && mHandleVBO &&
          mGrooveShader.Link() && mHandleShader.Link();
}

