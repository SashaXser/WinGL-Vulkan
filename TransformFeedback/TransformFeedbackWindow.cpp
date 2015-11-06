// local includes
#include "TransformFeedbackWindow.h"

// wingl includes
#include <Pipeline.h>
#include <WglAssert.h>

// gl includes
#include <GL/glew.h>
#include <GL/GL.h>

// std includes
#include <limits>

TransformFeedbackWindow::TransformFeedbackWindow( ) :
mpActiveControlPoint    ( nullptr )
{
}

TransformFeedbackWindow::~TransformFeedbackWindow( )
{
}

void TransformFeedbackWindow::OnDestroy( )
{
   // should still have a valid context
   WGL_ASSERT(ContextIsCurrent());

   // clean up resources
   mGenCurveShader = ShaderProgram();
   mVisCurveShader = ShaderProgram();

   mFBOCanvas = FrameBufferObject();

   mTFBGenCurve = VBO();

   // call the base class to clean things up
   OpenGLWindow::OnDestroy();
}

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
      mPipeline.EnableCullFace(true);
      mPipeline.EnableDepthTesting(true);

      // attach the visualization shaders for the curve
      mVisCurveShader.AttachFile(GL_VERTEX_SHADER, "transform_feedback_vis.vert");
      mVisCurveShader.AttachFile(GL_FRAGMENT_SHADER, "transform_feedback_vis.frag");

      // make sure the operation is complete
      if (!mVisCurveShader.Link())
      {
         // issue an error from the application that it could not link the files
         PostDebugMessage(GL_DEBUG_TYPE_ERROR, 1 | gl::debug::DISPLAY_MESSAGE_BOX_BIT, GL_DEBUG_SEVERITY_HIGH, "Unable to link shaders to visualize curve!!!");

         return false;
      }

      // generate the transform feedback object to capture state
      mTFOGenCurve.GenBuffer();
      mTFOGenCurve.Bind();

      // generate the buffer to hold the newly created data
      // let gl know that this is dynamic and will be used only in drawing
      mTFBGenCurve.GenBuffer(GL_TRANSFORM_FEEDBACK_BUFFER);
      mTFBGenCurve.Bind();
      mTFBGenCurve.BufferStorage(4 * sizeof(float) * 512, nullptr, 0);
      mTFBGenCurve.BindBufferBase(0);

      // no longer need to capture the feedback state
      mTFOGenCurve.Unbind();

      // attach the generation transform feedback shaders
      mGenCurveShader.AttachFile(GL_VERTEX_SHADER, "transform_feedback_gen.vert");
      mGenCurveShader.AttachFile(GL_GEOMETRY_SHADER, "transform_feedback_gen.geom");

      // indicate the type of attributes to record into the buffer
      // only need to use the geometry shaders gl_Position value
      const char * const varyings[] = { "gl_Position" };
      mGenCurveShader.TransformFeedbackVaryings(varyings, 1, GL_INTERLEAVED_ATTRIBS);

      // make sure the operation is complete
      if (!mGenCurveShader.Link())
      {
         // issue an error from the application that it could not link the files
         PostDebugMessage(GL_DEBUG_TYPE_ERROR, 1 | gl::debug::DISPLAY_MESSAGE_BOX_BIT, GL_DEBUG_SEVERITY_HIGH, "Unable to link shaders to generate curve!!!");

         return false;
      }

      // define the initial location of the control points
      mControlPoints = { Vec4f(-9.0f, -9.0f, 0.0f, 1.0f),
                         Vec4f(-5.0f, 9.0f, 0.0f, 1.0f),
                         Vec4f(5.0f, 9.0f, 0.0f, 1.0f),
                         Vec4f(9.0f, -9.0f, 0.0f, 1.0f) };

      // let the shader have the initial location
      mGenCurveShader.Enable();
      mGenCurveShader.SetUniformValue< 3 >("control_points",
                                           static_cast< const float * >(mControlPoints.front()),
                                           mControlPoints.size());
      mGenCurveShader.Disable();

      // add the control point locations to the vbo
      mVBOControlPoints.GenBuffer(GL_ARRAY_BUFFER);
      mVBOControlPoints.Bind();
      mVBOControlPoints.BufferStorage(mControlPoints.size() * sizeof(decltype(mControlPoints)::value_type),
                                      mControlPoints.front(),
                                      GL_DYNAMIC_STORAGE_BIT);
      mVBOControlPoints.Unbind();

      // add the indices for the four points
      mVBOControlPointsIndices.GenBuffer(GL_ELEMENT_ARRAY_BUFFER);
      mVBOControlPointsIndices.Bind();
      const uint32_t indices[] = { 0, 1, 2, 3 };
      mVBOControlPointsIndices.BufferStorage(sizeof(indices), indices, 0);
      mVBOControlPointsIndices.Unbind();

      // unbind the transform feedback
      mTFBGenCurve.Unbind();

      // generate the simple vao
      mTFAGenCurve.GenArray();

      // generate the canvas to render into
      mFBOCanvas.GenBuffer(nWidth, nHeight);
      mFBOCanvas.Bind(GL_FRAMEBUFFER);
      // attach two color buffers, one a true color buffer and
      // the other a selection buffer of integers...
      mFBOCanvas.Attach(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, GL_RGBA8);
      mFBOCanvas.Attach(GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, GL_R32UI);
      mFBOCanvas.Unbind();

      // post a message to load the correct settings of the shaders
      SendMessage(GetHWND(), WM_SIZE, 0, nHeight << 16 | nWidth);
      
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
         // clear the main color buffer...
         const GLfloat BLACK[] = { 0.0f, 0.0f, 0.0f, 1.0f };
         mPipeline.DrawBuffers({ GL_BACK_LEFT });
         mPipeline.ClearBuffer(GL_COLOR, 0, BLACK);

         // indicate that fragment operations are not to be performed...
         mPipeline.EnableRasterDiscard(true);

         // setup the transform feedback operation
         // this object will setup all the state associated for the operation
         mTFOGenCurve.Bind();

         // eanble the line generation
         mGenCurveShader.Enable();

         // begin the transform feedback
         mPipeline.EnableTransformFeedback(true, GL_POINTS);

         // draw a single point...
         // there are currently no bound buffers other than the feedback buffer...
         // the vertex shader passes a single point along and the remaining operations
         // will generate the line in the geometry shader upon executing the emit...
         mTFAGenCurve.Bind();
         mPipeline.DrawArrays(GL_POINTS, 0, 1);
         mTFAGenCurve.Unbind();

         // no longer need transform feedback operations
         mPipeline.EnableTransformFeedback(false);

         // disable collecting transform feedback data into the specified buffer
         mTFOGenCurve.Unbind();

         // disable the line generating shader
         mGenCurveShader.Disable();

         // allow fragment operations to continue like normal
         mPipeline.EnableRasterDiscard(false);

         // enable the color output destination buffers
         mFBOCanvas.Bind(GL_FRAMEBUFFER);

         // enable writing into the first buffer, disable the second
         mPipeline.DrawBuffers({ GL_COLOR_ATTACHMENT0, GL_NONE });

         // clear the main color attachment
         mPipeline.ClearBuffer(GL_COLOR, 0, BLACK);

         // enable the color buffer writing shader
         mVisCurveShader.Enable();

         // enable vertex stream processing for attribute 0
         mPipeline.EnableVertexAttribArray(true, 0);

         // bind the feedback buffer as an array buffer for rendering
         mTFBGenCurve.Bind(GL_ARRAY_BUFFER);
         mTFBGenCurve.VertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

         // begin rendering back the content of the transform feedback
         mPipeline.DrawTransformFeedback(GL_LINE_STRIP, mTFOGenCurve);

         // no longer need the feedback buffer
         mTFBGenCurve.Unbind();

         // enable writing into both buffers
         mPipeline.DrawBuffers({ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 });

         // clear the selection color attachment
         const GLuint MAX_GL_UINT = std::numeric_limits< GLuint >::max();
         mPipeline.ClearBuffer(GL_COLOR, 1, &MAX_GL_UINT);

         // bind the control points for rendering
         mVBOControlPoints.Bind();
         mVBOControlPointsIndices.Bind();
         mVBOControlPoints.VertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
         // render the control points
         mPipeline.DrawElements(GL_POINTS, mVBOControlPointsIndices.Size< uint32_t >(), GL_UNSIGNED_INT, nullptr);

         // enable vertex stream processing for attribute 0
         mPipeline.EnableVertexAttribArray(false, 0);

         // no longer rendering the control points
         mVBOControlPointsIndices.Unbind();
         mVBOControlPoints.Unbind();

         // disable the visualization shader
         mVisCurveShader.Disable();

         // disable the color output destination buffers
         mFBOCanvas.Unbind();

         // blit the contents of the first buffer to the default frame buffer
         mFBOCanvas.Bind(GL_READ_FRAMEBUFFER);
         uint8_t pixels[100*100*4] = {};
         glReadBuffer(GL_COLOR_ATTACHMENT0);
         glReadPixels(0, 0, 100, 100, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
         mFBOCanvas.Blit(0, 0, mFBOCanvas.Width(), mFBOCanvas.Height(),
                         0, 0, mFBOCanvas.Width(), mFBOCanvas.Height(),
                         GL_COLOR_ATTACHMENT0, GL_COLOR_BUFFER_BIT, GL_NEAREST);
         mFBOCanvas.Unbind();


         SwapBuffers(GetHDC());

//         //glEnable(GL_TEXTURE_2D);
//         gFBO.GetAttachment(GL_COLOR_ATTACHMENT1)->Bind(GL_TEXTURE0);
//         int iformat, rtype, rsize;
//         glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &iformat);
//         glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_RED_TYPE, &rtype);
//         glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_RED_SIZE, &rsize);
//         gFBO.GetAttachment(GL_COLOR_ATTACHMENT1)->Unbind();
//         //glDisable(GL_TEXTURE_2D);
//
//         gFBO.Bind(GL_READ_FRAMEBUFFER);
//         gFBO.IsComplete();
//         gFBO.GetCurrentFrameBuffer(GL_READ_FRAMEBUFFER);
//         glReadBuffer(GL_COLOR_ATTACHMENT1);
//         std::vector< uint32_t > test_collect(gFBO.Width() * gFBO.Height(), 0);
//         glReadPixels(0, 0, gFBO.Width(), gFBO.Height(), GL_RED_INTEGER, GL_UNSIGNED_INT, &test_collect.at(0));
//         //gFBO.GetAttachment(GL_COLOR_ATTACHMENT1)->Bind();
//         //glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, &test_collect.at(0));
//         //gFBO.GetAttachment(GL_COLOR_ATTACHMENT1)->Unbind();
//         gFBO.Unbind();
//         glReadBuffer(GL_BACK);

         //static int i = 1000;

         //if (--i == 0)
         //{
         //   const auto width = gFBO.Width();
         //   const auto height = gFBO.Height();

         //   for (int h = gFBO.Height() - 1; h >= 0; --h)
         //   {
         //      const auto row = width * h;

         //      for (int w = 0; w < gFBO.Width(); ++w)
         //      {
         //         if (test_collect[row + w] == std::numeric_limits< GLuint >::max())
         //         {
         //            std::cout << ".";
         //         }
         //         else
         //         {
         //            std::cout << test_collect[row + w];
         //         }

         //         if (test_collect[row + w])
         //         {
         //            uint32_t blah = test_collect[row + w];
         //            blah *= 2;
         //         }
         //      }
         //      std::cout << std::endl;
         //   }

         //   std::cout << std::endl;
         //   std::cout << std::endl;

         //   i = 1000;
         //}
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
   {
      // obtain the width and height
      const uint32_t width = lParam & 0xFFFF;
      const uint32_t height = lParam >> 16;

      // update the viewport
      glViewport(0, 0,
                 static_cast< GLsizei >(width),
                 static_cast< GLsizei >(height));

      if (mVisCurveShader)
      {
         // update the information in the shaders
         // the application will have a fixed sized view
         const Matrixf projection = Matrixd::Ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f);
         const Matrixf mv = Matrixd::LookAt(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

         // update the internal matrix for proper projection
         mVisCurveShader.Enable();
         mVisCurveShader.SetUniformMatrix< 1, 4, 4 >("model_view_proj_mat", projection * mv);
         mVisCurveShader.Disable();
      }

      // resize the canvas to match the new dims
      if (mFBOCanvas &&
         (mFBOCanvas.Width() != width ||
          mFBOCanvas.Height() != height))
      {
         mFBOCanvas.Bind(GL_FRAMEBUFFER);
         mFBOCanvas.Resize(GetSize().width, GetSize().height);
         mFBOCanvas.Unbind();
      }
   }

   break;

//   case WM_MOUSEMOVE:
//      if (pActivePoint)
//      {
//         const auto x_screen = static_cast< intptr_t >(lParam & 0xFFFF);
//         const auto y_screen = GetSize().height - static_cast< intptr_t >(lParam >> 16);
//
//         const auto x_world = 10.0 * ((2.0 * x_screen / GetSize().width) - 1.0);
//         const auto y_world = 10.0 * ((2.0 * y_screen / GetSize().height) - 1.0);
//
//         pActivePoint->Set(x_world, y_world, 0.0f);
//      }
//
//      break;
//
//   case WM_LBUTTONDOWN:
//   {
//      gFBO.Bind(GL_READ_FRAMEBUFFER);
//      gFBO.GetCurrentFrameBuffer(GL_READ_FRAMEBUFFER);
//      glReadBuffer(GL_COLOR_ATTACHMENT1);
//      std::vector< uint32_t > test_collect(gFBO.Width() * gFBO.Height(), 0);
//      glReadPixels(0, 0, gFBO.Width(), gFBO.Height(), GL_RED_INTEGER, GL_UNSIGNED_INT, &test_collect.at(0));
//
//      const auto x = static_cast< intptr_t >(lParam & 0xFFFF);
//      const auto y = static_cast< intptr_t >(lParam >> 16);
//
//      const auto selection = test_collect[(GetSize().height - y) * GetSize().width + x];
//
//      if (selection != std::numeric_limits< GLuint >::max())
//      {
//         pActivePoint = &mControlPoints[selection];
//         mGenCurveShader.Enable();
//         mGenCurveShader.SetUniformValue< 3 >("control_points", static_cast< const float * >(points.front()), points.size());
//         mGenCurveShader.Disable();
//      }
//   }

      break;

   case WM_LBUTTONUP:
      // release the active point...
//      pActivePoint = nullptr;

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
