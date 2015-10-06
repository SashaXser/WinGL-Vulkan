// local includes
#include "AutoScaleWindow.h"

// wgl includes
#include "Matrix.h"
#include "Vector.h"
#include "MathHelper.h"

// gl includes
#include <GL/glew.h>
#include <GL/GL.h>

// std incluces
#include <cmath>
#include <cstdint>
#include <iostream>

AutoScaleWindow::AutoScaleWindow( )
{
}

AutoScaleWindow::~AutoScaleWindow( )
{
   
}

bool AutoScaleWindow::Create( unsigned int nWidth,
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
      mCamera.LookAt({ 0.0f, 0.0f, 30.0f }, { 0.0f, 0.0f, 0.0f });

      // enable the depth buffer and cull faces
      glEnable(GL_CULL_FACE);
      glEnable(GL_DEPTH_TEST);

      // init all things gl
      InitGLData();
      
      // force the projection matrix to get calculated and updated
      SendMessage(GetHWND(), WM_SIZE, 0, nHeight << 16 | nWidth);

      // indicate what actions can be taken
      std::cout << std::endl
                << "a - Moves camera to the left" << std::endl
                << "d - Moves camera to the right" << std::endl
                << "w - Moves camera to the up" << std::endl
                << "s - Moves camera to the down" << std::endl
                << std::endl
                << "L-Button Down - Activate camera rotation" << std::endl
                << "Mouse X / Y Delta - Manipulate camera rotation" << std::endl;

      return true;
   }

   return false;
}

void AutoScaleWindow::OnDestroy( )
{
   // call the base class destroy
   OpenGLWindow::OnDestroy();
}
            
int AutoScaleWindow::Run( )
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

         // use the basic shader to render
         mBasicShader.Enable();

         // set the shape color
         mBasicShader.SetUniformValue("shape_color", 1.0f, 1.0f, 1.0f);

         // draw the square
         mAutoScaleShape.Bind();
         glDrawArrays(GL_LINE_STRIP, 0, 4);
         mAutoScaleShape.Unbind();

         // set the shape color
         mBasicShader.SetUniformValue("shape_color", 1.0f, 0.0f, 0.0f);

         // get the current value of the model view projection matrix
         Matrixf auto_scale_shape_mvp;
         mBasicShader.GetUniformValue("model_view_proj", auto_scale_shape_mvp);

         // reset the model view projection matrix
         mBasicShader.SetUniformMatrix< 1, 4, 4 >("model_view_proj", mCamera.GetProjectionMatrix() * mCamera.GetViewMatrix());

         // draw the floor
         mFloorShape.Bind();
         glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
         glDrawArrays(GL_TRIANGLE_STRIP, 6, 6);
         mFloorShape.Unbind();

         // reset the model view projection matrix for the auto scale shape
         mBasicShader.SetUniformMatrix< 1, 4, 4 >("model_view_proj", auto_scale_shape_mvp);

         // no longer using the shader
         mBasicShader.Disable();

         // swap the front and back buffers
         SwapBuffers(GetHDC());
      }
   }

   return appQuitVal;
}

LRESULT AutoScaleWindow::MessageHandler( UINT uMsg,
                                         WPARAM wParam,
                                         LPARAM lParam )
{
   LRESULT result = 0;

   bool update_view_proj_matrix = false;

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
      mCamera.SetPerspective(45.0f, static_cast< float >(width) / static_cast< float >(height), 0.01f, 1000.0f);

      // need to update the view projection matrix
      update_view_proj_matrix = true;
      }

      break;

   case WM_CHAR:
      // process key presses
      switch (wParam)
      {
      case 'a':
      case 'd':
         // stafe the camera left or right
         mCamera.TranslateRight(wParam == 'a' ? -0.5f : 0.5f);

         // need to update the view projection matrix
         update_view_proj_matrix = true;

         break;

      case 'w':
      case 's':
         // move the camera forwards or backwards
         mCamera.TranslateForward(wParam == 'w' ? -0.5f : 0.5f);

         // need to update the view projection matrix
         update_view_proj_matrix = true;

         break;
      }

      break;

   case WM_MOUSEMOVE:
      {

      // get current mouse positions
      const auto current_mouse_x = static_cast< intptr_t >(lParam & 0xFFFF);
      const auto current_mouse_y = static_cast< intptr_t >(lParam >> 16);

      if (wParam & MK_LBUTTON)
      {
         // get the delta between current and previous positions
         const auto delta_x = current_mouse_x - GetPreviousMousePosition().x;
         const auto delta_y = current_mouse_y - GetPreviousMousePosition().y;

         // rotate the camera
         mCamera.RotateYaw(delta_x * 0.05f);
         mCamera.RotatePitch(delta_y * 0.05f);

         // need to update the view projection matrix
         update_view_proj_matrix = true;
      }

      }

      break;

   default:
      // allow default processing to happen
      result = OpenGLWindow::MessageHandler(uMsg, wParam, lParam);
   }

   if (mBasicShader && update_view_proj_matrix)
   {
      // the auto scale calculation can be easily defined in a shader

      // calculate the model matrix
      const Vec3f eye = mCamera.GetEyePosition();
      // uncomment this eye_dir to do billboarding that faces the camera
      //const Vec3f eye_dir = eye.UnitVector();
      const Vec3f eye_dir = Vec3f(mCamera.GetViewMatrix()[2], mCamera.GetViewMatrix()[6], mCamera.GetViewMatrix()[10]).UnitVector();
      const Vec3f right_dir = (Vec3f(0.0f, 1.0f, 0.0f) ^ eye_dir).UnitVector();
      const Vec3f up_dir = (eye_dir ^ right_dir).UnitVector();

      // construct a matrix that defines the models rotation
      const Matrixf model_matrix(Vec4f(right_dir, 0.0f),
                                 Vec4f(up_dir, 0.0f),
                                 Vec4f(eye_dir, 0.0f),
                                 Vec4f(0.0f, 0.0f, 0.0f, 1.0f));

      // project the vector from the camera to the object onto the eye_dir vector
      const float distance = ((eye_dir * -1.0f) * ((eye * -1.0f) * eye_dir)).Length();

      // calculate the ratio of a height at distance 1 and the height at the near plane
      const float ratio = std::tan(math::DegToRad(mCamera.GetProjectionFOV()) / 2.0f) / (GetSize().height / 2.0f);

      // the scale matrix is defined by the distance * ratio
      const Matrixf scale_matrix = Matrixf::Scale(distance * ratio);

      // update the matrix in the basic shader
      mBasicShader.Enable();
      mBasicShader.SetUniformMatrix< 1, 4, 4 >("model_view_proj", mCamera.GetProjectionMatrix() * mCamera.GetViewMatrix() * model_matrix * scale_matrix);
      mBasicShader.Disable();
   }
   
   return result;
}

void AutoScaleWindow::InitGLData( )
{
   // enable the shape data
   mAutoScaleShape.GenArray();
   mAutoScaleShape.Bind();

   // generate the vertex data
   mAutoScaleShapeVerts.GenBuffer(GL_ARRAY_BUFFER);
   mAutoScaleShapeVerts.Bind();
   const float shape_verts[] = { -20.0f, -20.0f, 0.0f, 20.0f, -20.0f, 0.0f, 20.0f, 20.0f, 0.0f, -20.0f, 20.0f, 0.0f };
   mAutoScaleShapeVerts.BufferData(sizeof(shape_verts), shape_verts, GL_STATIC_DRAW);
   mAutoScaleShapeVerts.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
   mAutoScaleShapeVerts.Unbind();

   // enable the vertex positions
   mAutoScaleShape.EnableVertexAttribArray(0);

   // no longer need the shape data bound
   mAutoScaleShape.Unbind();

   // enable the floor shape
   mFloorShape.GenArray();
   mFloorShape.Bind();

   // generate the vertex data
   mFloorVerts.GenBuffer(GL_ARRAY_BUFFER);
   mFloorVerts.Bind();
   const float floor_verts[] = { -25.0f, 0.0f, 0.0f, -25.0f, 0.0f, 25.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 25.0f, 25.0f, 0.0f, 0.0f, 25.0f, 0.0f, 25.0f,
                                 -25.0f, 0.0f, -25.0f, -25.0f, 0.0f, 0.0f, 0.0f, 0.0f, -25.0f, 0.0f, 0.0f, 0.0f, 25.0f, 0.0f, -25.0f, 25.0f, 0.0f, 0.0f };
   mFloorVerts.BufferData(sizeof(floor_verts), floor_verts, GL_STATIC_DRAW);
   mFloorVerts.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
   mFloorVerts.Unbind();

   // enable the vertex positions
   mFloorShape.EnableVertexAttribArray(0);

   // no longer need the floor shape bound
   mFloorShape.Unbind();

   // generate the basic shader
   mBasicShader.Attach(GL_VERTEX_SHADER,
                       "#version 410\n"
                       "layout (location = 0) in vec3 vertex_position;\n"
                       "uniform mat4 model_view_proj;\n"
                       "\n"
                       "void main( )\n"
                       "{\n"
                       "   gl_Position = model_view_proj * vec4(vertex_position, 1.0f);\n"
                       "}\n");
   mBasicShader.Attach(GL_FRAGMENT_SHADER,
                       "#version 410\n"
                       "layout (location = 0) out vec4 frag_color_dest_0;\n"
                       "uniform vec3 shape_color;\n"
                       "\n"
                       "void main( )\n"
                       "{\n"
                       "   frag_color_dest_0 = vec4(shape_color, 1.0f);\n"
                       "}\n");
   // link the basic shader
   mBasicShader.Link();
}

