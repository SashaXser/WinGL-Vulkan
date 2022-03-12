// local includes
#include "DOFTransformWindow.h"

// wgl includes
#include "Matrix.h"
//#include "Vector.h"

// gl includes
#include <GL/glew.h>
#include <GL/GL.h>

// std incluces
//#include <cmath>
//#include <cstdint>
//#include <iostream>

DOFTransformWindow::DOFTransformWindow( )
{
}

DOFTransformWindow::~DOFTransformWindow( )
{
   
}

bool DOFTransformWindow::Create( unsigned int nWidth,
                                 unsigned int nHeight,
                                 const char * pWndTitle,
                                 const void * /*pInitParams*/ )
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

      // init all things gl
      InitGLData();
      
      // force the projection matrix to get calculated and updated
      SendMessage(GetHWND(), WM_SIZE, 0, nHeight << 16 | nWidth);

      // indicate what actions can be taken
//      std::cout << std::endl
//                << "a - Moves camera to the left" << std::endl
//                << "d - Moves camera to the right" << std::endl
//                << "w - Moves camera to the up" << std::endl
//                << "s - Moves camera to the down" << std::endl
//                << std::endl
//                << "L-Button Down - Activate camera rotation" << std::endl
//                << "Mouse X / Y Delta - Manipulate camera rotation" << std::endl;

      return true;
   }

   return false;
}

void DOFTransformWindow::OnDestroy( )
{
   // call the base class destroy
   OpenGLWindow::OnDestroy();
}
            
int DOFTransformWindow::Run( )
{
   // vars for the loop
   int appQuitVal = 0;
   bool bQuit = false;

   // basic message pump and render frame
   while (!bQuit)
   {
      // process all the messages
      bQuit = PeekAppMessages(appQuitVal);

      if (!bQuit)
      {
         // clear the back buffer and the depth buffer
         glClear(GL_COLOR_BUFFER_BIT);

         // use the basic shader to render
         mBasicShader.Enable();

         // set the shape color
         mBasicShader.SetUniformValue("shape_color", 1.0f, 1.0f, 1.0f);

         // determine the projection and view matrix
         const Matrixf projection = Matrixf::Ortho(-10.0f, 10.0f, -10.0f, 10.0f, 15.0f, -15.0f);
         const Matrixf view = Matrixf::LookAt(0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

         // update the basic shader
         mBasicShader.SetUniformMatrix< 1, 4, 4 >("model_view_proj", projection * view);
         
         // draw the square
         mBodyShape.Bind();
         glDrawArrays(GL_LINE_LOOP, 0, 5);
         mBodyShape.Unbind();

         // set the shape color
         mBasicShader.SetUniformValue("shape_color", 1.0f, 0.0f, 0.0f);

         // determine the turret rotation
         static float degrees = 0.0; degrees += 0.01f;
         const Matrixf put = Matrixf::Translate(0.0f, 1.25f, 0.0f);
         const Matrixf yaw = Matrixf::Rotate(degrees, 0.0f, 0.0f, 1.0f);

         // update the turret location
         mBasicShader.SetUniformMatrix< 1, 4, 4 >("model_view_proj", projection * view * put.Inverse() * yaw * put);

         // draw the floor
         mTurretShape.Bind();
         glDrawArrays(GL_LINE_LOOP, 0, 4);
         glDrawArrays(GL_LINE_LOOP, 4, 4);
         mTurretShape.Unbind();

         // no longer using the shader
         mBasicShader.Disable();

         // swap the front and back buffers
         SwapBuffers(GetHDC());
      }
   }

   return appQuitVal;
}

LRESULT DOFTransformWindow::MessageHandler( UINT uMsg,
                                            WPARAM wParam,
                                            LPARAM lParam )
{
   LRESULT result = 0;

   switch (uMsg)
   {
   case WM_CHAR:
      // process key presses
      switch (wParam)
      {
      case 'a':
      case 'd':

         break;

      case 'w':
      case 's':

         break;
      }

      break;

   default:
      // allow default processing to happen
      result = OpenGLWindow::MessageHandler(uMsg, wParam, lParam);
   }
   
   return result;
}

void DOFTransformWindow::InitGLData( )
{
   // enable the shape data
   mBodyShape.GenArray();
   mBodyShape.Bind();

   // generate the vertex data
   mBodyShapeVerts.GenBuffer(GL_ARRAY_BUFFER);
   mBodyShapeVerts.Bind();
   const float body_verts[] = { -2.5f, 0.0f, 0.0f, -2.5f, -2.5f, 0.0f, 0.0f, -5.0f, 0.0f, 2.5f, -2.5f, 0.0f, 2.5f, 0.0f, 0.0f };
   mBodyShapeVerts.BufferData(sizeof(body_verts), body_verts, GL_STATIC_DRAW);
   mBodyShapeVerts.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
   mBodyShapeVerts.Unbind();

   // enable the vertex positions
   mBodyShape.EnableVertexAttribArray(0);

   // no longer need the shape data bound
   mBodyShape.Unbind();

   // enable the floor shape
   mTurretShape.GenArray();
   mTurretShape.Bind();

   // generate the vertex data
   mTurretShapeVerts.GenBuffer(GL_ARRAY_BUFFER);
   mTurretShapeVerts.Bind();
   const float turret_verts[] = { -2.5f, 0.0f, 0.0f, -2.5f, -2.5f, 0.0f, 2.5f, -2.5f, 0.0f, 2.5f, 0.0f, 0.0f,
                                  -0.5f, 5.0f, 0.0f, -0.5f,  0.0f, 0.0f, 0.5f,  0.0f, 0.0f, 0.5f, 5.0f, 0.0f };
   mTurretShapeVerts.BufferData(sizeof(turret_verts), turret_verts, GL_STATIC_DRAW);
   mTurretShapeVerts.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
   mTurretShapeVerts.Unbind();

   // enable the vertex positions
   mTurretShape.EnableVertexAttribArray(0);

   // no longer need the floor shape bound
   mTurretShape.Unbind();

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

