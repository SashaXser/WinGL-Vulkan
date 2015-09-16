// local includes
#include "TransformFeedbackWindow.h"

// wingl includes
#include <WglAssert.h>

// gl includes
#include <GL/glew.h>
#include <GL/GL.h>

TransformFeedbackWindow::TransformFeedbackWindow( ) :
mpTrianglesShader    ( new ShaderProgram )
{
}

TransformFeedbackWindow::~TransformFeedbackWindow( )
{
}

void TransformFeedbackWindow::OnDestroy( )
{
   // should still have a valid context
   WGL_ASSERT(ContextIsCurrent());

   // cleanup the resources
   mpTrianglesShader = nullptr;

   // call the base class to clean things up
   OpenGLWindow::OnDestroy();
}

// temp
#include <GeomHelper.h>
#include <Matrix.h>
#include <Timer.h>
VAO gVAO;
VBO gVBO;
VBO gIVBO;
VBO gTFB;

bool TransformFeedbackWindow::Create( unsigned int nWidth,
                                      unsigned int nHeight,
                                      const char * pWndTitle,
                                      const void * pInitParams )
{
   // major, minor, compatible, debug, forward compatible
   const OpenGLWindow::OpenGLInit glInit[] =
   {
      { 4, 0, true, true, false }, 
      { 0 }
   };

   // call base class to init
   if (OpenGLWindow::Create(nWidth, nHeight, pWndTitle, glInit))
   {
      // make the context current
      MakeCurrent();

      // attach to the debug context
      AttachToDebugContext();

      // enable specific state
      glEnable(GL_CULL_FACE);
      glEnable(GL_DEPTH_TEST);



      mpTrianglesShader->AttachFile(GL_VERTEX_SHADER, "transform_feedback.vert");
      mpTrianglesShader->AttachFile(GL_GEOMETRY_SHADER, "transform_feedback.geom");
      mpTrianglesShader->AttachFile(GL_FRAGMENT_SHADER, "transform_feedback.frag");


      gTFB.GenBuffer(GL_TRANSFORM_FEEDBACK_BUFFER);
      gTFB.Bind();
      gTFB.BufferStorage(4 * sizeof(float) * 512, nullptr, GL_MAP_READ_BIT);
      //glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, transform_buffer);

      char * varyings[] = { "gl_Position" };
      mpTrianglesShader->TransformFeedbackVaryings(varyings, 1, GL_INTERLEAVED_ATTRIBS);

      mpTrianglesShader->Link();

      gTFB.Unbind();

      //const auto plane = GeomHelper::ConstructPlane(5.0f, 5.0f);
      gVAO.GenArray();
      gVAO.Bind();

      //gVBO.GenBuffer(GL_ARRAY_BUFFER);
      //gVBO.Bind();
      //std::vector< Vec3f > points = { Vec3f(-5.0f, 0.0f, 0.0f), Vec3f(0.0f, 10.0f, 0.0f), Vec3f(5.0f, 0.0f, 0.0f) };
      //gVBO.BufferData(points.size() * sizeof(Vec3f), points.front(), GL_STATIC_DRAW);
      //gVBO.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
      //gVAO.EnableVertexAttribArray(0);
      //gVBO.Unbind();

      //gIVBO.GenBuffer(GL_ELEMENT_ARRAY_BUFFER);
      //gIVBO.Bind();
      //gIVBO.BufferData(plane.indices.size() * sizeof(GLuint), &plane.indices[0], GL_STATIC_DRAW);

      gVAO.Unbind();

      //gIVBO.Unbind();
      
      return true;
   }
   else
   {
      // issue an error from the application that it could not be created
      PostDebugMessage(GL_DEBUG_TYPE_ERROR, 1 | gl::debug::DISPLAY_MESSAGE_BOX_BIT, GL_DEBUG_SEVERITY_HIGH, "Unable To Create 4.0 OpenGL Context");
   }

   return false;
}

int TransformFeedbackWindow::Run( )
{
   // app quit variables
   int appQuitVal = 0;
   bool bQuit = false;

   while (!bQuit)
   {
      // process all the app messages and then render the scene
      if (!(bQuit = PeekAppMessages(appQuitVal)))
      {
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

         //glMatrixMode(GL_PROJECTION);
         Matrixd projection = Matrixd::Ortho(-10.0, 10.0, -10.0, 10.0, -10.0, 10.0);
         //glLoadMatrixd(projection);

         //glMatrixMode(GL_MODELVIEW);
         Matrixd mv = Matrixd::LookAt(0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
         //glLoadMatrixd(mv);

         //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

         glColor3f(1,1,1);

         mpTrianglesShader->Enable();
         mpTrianglesShader->SetUniformMatrix<1, 4, 4 >("model_view", static_cast< Matrixf >(mv));
         mpTrianglesShader->SetUniformMatrix<1, 4, 4 >("model_view_proj_mat", static_cast< Matrixf >(projection * mv));

         const std::vector< Vec3f > points = [ ] ( )
         {
            const double current_time = Timer().GetCurrentTimeSec();

            double cos = std::cos(current_time);
            double sin = std::sin(current_time);

            const Vec3f pt1(0.0f + 10 * cos, 0.0f, 0.0f);

            cos = std::cos(current_time + 1.0);
            sin = std::sin(current_time + 1.0);

            const Vec3f pt2(cos * 10, cos * -10, 0.0f);

            cos = std::cos(current_time + 2.0);
            sin = std::sin(current_time + 2.0);

            const Vec3f pt3(5.0f, -2.0f + 8 * cos, 0.0f);

            return std::vector< Vec3f > { pt1, pt2, pt3 };
            //{ Vec3f(-5.0f, -5.0f, 0.0f), Vec3f(0.0f, 5.0f, 0.0f), Vec3f(5.0f, -2.0f, 0.0f) };
         }();
         mpTrianglesShader->SetUniformValue< 3 >("control_points", static_cast< const float * >(points.front()), 3);

         glEnable(GL_RASTERIZER_DISCARD);

         //glEnableClientState(GL_VERTEX_ARRAY);
         //gVBO.Bind();
         //glVertexPointer(3, GL_FLOAT, 0, nullptr);

         gTFB.Bind();
         gTFB.BindBufferBase(0);
         glBeginTransformFeedback(GL_POINTS);

         //gIVBO.Bind();

         gVAO.Bind();
         //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
         glDrawArrays(GL_POINTS, 0, 1);
         gVAO.Unbind();

         glEndTransformFeedback();

         //gIVBO.Unbind();
         //gVBO.Unbind();
         //glDisableClientState(GL_VERTEX_ARRAY);

         glDisable(GL_RASTERIZER_DISCARD);

         mpTrianglesShader->Disable();

         const void * const pBuffer = gTFB.MapBuffer(GL_READ_ONLY);

         gTFB.Unbind();
         gTFB.UnbindBufferBase(0);



         glMatrixMode(GL_PROJECTION);
         glLoadMatrixd(projection);

         glMatrixMode(GL_MODELVIEW);
         glLoadMatrixd(mv);

         glEnableClientState(GL_VERTEX_ARRAY);
         glVertexPointer(4, GL_FLOAT, 0, pBuffer);
         glDrawArrays(GL_LINE_STRIP, 0, 512);
         glVertexPointer(3, GL_FLOAT, 0, points.front());
         glPointSize(5.0f);
         glDrawArrays(GL_POINTS, 0, 3);
         glPointSize(1.0f);
         glDisableClientState(GL_VERTEX_ARRAY);


         gTFB.Bind();
         gTFB.BindBufferBase(0);
         gTFB.UnmapBuffer();
         gTFB.Unbind();
         gTFB.UnbindBufferBase(0);
         

         SwapBuffers(GetHDC());
      }
   }

   return appQuitVal;
}


LRESULT TransformFeedbackWindow::MessageHandler( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
   LRESULT result = 0;

   switch (uMsg)
   {
   case WM_SIZE:
      // update the viewport
      glViewport(0, 0,
                 static_cast< GLsizei >(lParam & 0xFFFF),
                 static_cast< GLsizei >(lParam >> 16));

      break;

   case WM_MOUSEMOVE:

      break;

   case WM_MOUSEWHEEL:

      break;

   case WM_CHAR:

      break;

   default:
      // default handle the messages
      result = OpenGLWindow::MessageHandler(uMsg, wParam, lParam);
   }

   return result;
}
