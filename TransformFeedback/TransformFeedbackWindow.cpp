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
#include <QueryObject.h>
#include <FrameBufferObject.h>
VAO gVAO;
VBO gVBO;
VBO gIVBO;
VBO gTFB;
QueryObject gQO;
FrameBufferObject gFBO;

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
      //mpTrianglesShader->AttachFile(GL_FRAGMENT_SHADER, "transform_feedback.frag");


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

      gQO.GenQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);

      gFBO.GenBuffer(nWidth, nHeight);
      gFBO.Bind(GL_FRAMEBUFFER);
      gFBO.Attach(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, GL_RGBA8);
      gFBO.Attach(GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, GL_R32UI);
      gFBO.Unbind();
      
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

         gQO.Begin();

         //gIVBO.Bind();

         gVAO.Bind();
         //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
         glDrawArrays(GL_POINTS, 0, 1);
         gVAO.Unbind();

         gQO.End();

         glEndTransformFeedback();

         //gIVBO.Unbind();
         //gVBO.Unbind();
         //glDisableClientState(GL_VERTEX_ARRAY);

         glDisable(GL_RASTERIZER_DISCARD);

         mpTrianglesShader->Disable();

         const void * const pBuffer = gTFB.MapBuffer(GL_READ_ONLY);

         gTFB.Unbind();
         gTFB.UnbindBufferBase(0);


         // temp... move later
         ShaderProgram temp_shader;
         const char * const vert_source =
            "#version 400 compatibility\n"
            "void main( ) {\n"
            "gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;\n"
            "}";
         temp_shader.Attach(GL_VERTEX_SHADER, vert_source);
         temp_shader.AttachFile(GL_FRAGMENT_SHADER, "transform_feedback.frag");
         temp_shader.Link();
         temp_shader.Enable();


         glMatrixMode(GL_PROJECTION);
         glLoadMatrixd(projection);

         glMatrixMode(GL_MODELVIEW);
         glLoadMatrixd(mv);

         gFBO.Bind(GL_FRAMEBUFFER);

         const GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
         glDrawBuffers(2, buffers);

         if (gFBO.Width() != GetSize().width ||
             gFBO.Height() != GetSize().height)
         {
            gFBO.Resize(GetSize().width, GetSize().height);
         }

         //glClear(GL_COLOR_BUFFER_BIT);
         GLfloat black[] = {0, 0, 0, 1};
         glClearBufferfv(GL_COLOR, 0, black);
         GLuint zero = 0;
         glClearBufferuiv(GL_COLOR, 1, &zero);


         glEnableClientState(GL_VERTEX_ARRAY);
         glVertexPointer(4, GL_FLOAT, 0, pBuffer);
         const auto num_verts_written = gQO.Value< GLuint >();
         glDrawArrays(GL_LINE_STRIP, 0, num_verts_written);
         glVertexPointer(3, GL_FLOAT, 0, points.front());
         glPointSize(5.0f);
         glDrawArrays(GL_POINTS, 0, 3);
         glPointSize(1.0f);
         glDisableClientState(GL_VERTEX_ARRAY);

         gFBO.Unbind();

         temp_shader.Disable();

         // more than likely not needed
         //glDrawBuffer(GL_BACK);


         gTFB.Bind();
         gTFB.BindBufferBase(0);
         gTFB.UnmapBuffer();
         gTFB.Unbind();
         gTFB.UnbindBufferBase(0);

         
         const Vec3f fullscreen[] =
         {
            Vec3f(-10.0f, 10.0, 0.0f),
            Vec3f(-10.0f, -10.0f, 0.0f),
            Vec3f(10.0f, -10.0f, 0.0f),
            
            Vec3f(-10.0f, 10.0, 0.0f),
            Vec3f(10.0f, -10.0f, 0.0f),
            Vec3f(10.0f, 10.0f, 0.0f)
         };

         const Vec2f fullscreen_st[] =
         {
            Vec2f(0.0f, 1.0f),
            Vec2f(0.0f, 0.0f),
            Vec2f(1.0f, 0.0f),
            
            Vec2f(0.0f, 1.0f),
            Vec2f(1.0f, 0.0f),
            Vec2f(1.0f, 1.0f)
         };

         glEnableClientState(GL_VERTEX_ARRAY);
         glEnableClientState(GL_TEXTURE_COORD_ARRAY);
         glVertexPointer(3, GL_FLOAT, 0, fullscreen);
         glTexCoordPointer(2, GL_FLOAT, 0, fullscreen_st);

         glEnable(GL_TEXTURE_2D);
         gFBO.GetAttachment(GL_COLOR_ATTACHMENT0)->Bind(GL_TEXTURE0);

         glDrawArrays(GL_TRIANGLES, 0, sizeof(fullscreen) / sizeof(*fullscreen));

         gFBO.GetAttachment(GL_COLOR_ATTACHMENT0)->Unbind();
         glDisable(GL_TEXTURE_2D);

         glDisableClientState(GL_VERTEX_ARRAY);
         glDisableClientState(GL_TEXTURE_COORD_ARRAY);

         SwapBuffers(GetHDC());

         //glEnable(GL_TEXTURE_2D);
         gFBO.GetAttachment(GL_COLOR_ATTACHMENT1)->Bind(GL_TEXTURE0);
         int iformat, rtype, rsize;
         glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &iformat);
         glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_RED_TYPE, &rtype);
         glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_RED_SIZE, &rsize);
         gFBO.GetAttachment(GL_COLOR_ATTACHMENT1)->Unbind();
         //glDisable(GL_TEXTURE_2D);

         gFBO.Bind(GL_READ_FRAMEBUFFER);
         gFBO.IsComplete();
         gFBO.GetCurrentFrameBuffer(GL_READ_FRAMEBUFFER);
         glReadBuffer(GL_COLOR_ATTACHMENT1);
         std::vector< uint32_t > test_collect(gFBO.Width() * gFBO.Height(), 0);
         glReadPixels(0, 0, gFBO.Width(), gFBO.Height(), GL_RED_INTEGER, GL_UNSIGNED_INT, &test_collect.at(0));
         //gFBO.GetAttachment(GL_COLOR_ATTACHMENT1)->Bind();
         //glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, &test_collect.at(0));
         //gFBO.GetAttachment(GL_COLOR_ATTACHMENT1)->Unbind();
         gFBO.Unbind();
         glReadBuffer(GL_BACK);

         static int i = 1000;

         if (--i == 0)
         {
            const auto width = gFBO.Width();
            const auto height = gFBO.Height();

            for (int h = gFBO.Height() - 1; h >= 0; --h)
            {
               const auto row = width * h;

               for (int w = 0; w < gFBO.Width(); ++w)
               {
                  std::cout << (test_collect[row + w] == 1 ? "1" : test_collect[row + w] ? "X" : ".");
                  if (test_collect[row + w])
                  {
                     uint32_t blah = test_collect[row + w];
                     blah *= 2;
                  }
               }
               std::cout << std::endl;
            }

            std::cout << std::endl;
            std::cout << std::endl;

            i = 1000;
         }
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
