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
#include <string>
#include <iostream>

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
                                      const void * /*pInitParams*/ )
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

      // spit out the informational message
      std::cout << std::endl
                << "Keyboard Actions" << std::endl
                << "'3' - Activate three control points." << std::endl
                << "'4' - Activate four control points." << std::endl
                << "'5' - Activate five control points." << std::endl
                << "'6' - Activate six control points." << std::endl
                << "Mouse Actions" << std::endl
                << "Left Mouse Button Drag - Move a control point mouse is over." << std::endl;

      // enable specific state
      mPipeline.EnableCullFace(true);
      mPipeline.EnableDepthTesting(true);
      mPipeline.EnableProgramPointSize(true);

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
                         Vec4f(9.0f, -9.0f, 0.0f, 1.0f),
                         Vec4f(0.0f, 0.0f, 0.0f, 1.0f),
                         Vec4f(0.0f, 0.0f, 0.0f, 1.0f) };

      // let the shader have the initial location
      mGenCurveShader.Enable();
      mGenCurveShader.SetUniformValue< 4 >("control_points",
                                           static_cast< const float * >(mControlPoints.front()),
                                           static_cast< GLsizei >(mControlPoints.size()));
      mGenCurveShader.SetUniformValue("number_of_control_points", uint32_t(3));
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
      const uint32_t indices[] = { 0, 1, 2 };
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
      bQuit = PeekAppMessages(appQuitVal);

      if (!bQuit)
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
         mFBOCanvas.Blit(0, 0, mFBOCanvas.Width(), mFBOCanvas.Height(),
                         0, 0, mFBOCanvas.Width(), mFBOCanvas.Height(),
                         GL_COLOR_ATTACHMENT0, GL_COLOR_BUFFER_BIT, GL_NEAREST);
         mFBOCanvas.Unbind();

         // swap back with the front
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
   {
      // obtain the width and height
      const uint32_t width = static_cast< uint32_t >(lParam & 0xFFFF);
      const uint32_t height = static_cast< uint32_t >(lParam >> 16);

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

   case WM_MOUSEMOVE:
      // move the point around the screen if one is selected
      if (mpActiveControlPoint)
      {
         // get the current x and y screen space location
         const auto x_screen = static_cast< intptr_t >(lParam & 0xFFFF);
         const auto y_screen = GetSize().height - static_cast< intptr_t >(lParam >> 16);

         // convert the screen space to the world space...
         const auto x_world = 10.0f * ((2.0f * x_screen / GetSize().width) - 1.0f);
         const auto y_world = 10.0f * ((2.0f * y_screen / GetSize().height) - 1.0f);

         // set the new location...
         mpActiveControlPoint->Set(x_world, y_world, 0.0f, 1.0f);

         // update the shader's view of the control points
         mGenCurveShader.Enable();
         mGenCurveShader.SetUniformValue< 4 >(
            "control_points",
            static_cast< const float * >(mControlPoints.front()),
            static_cast< GLsizei >(mControlPoints.size()));
         mGenCurveShader.Disable();

         // update the buffer for the visual shader
         mVBOControlPoints.Bind();
         mVBOControlPoints.BufferSubData(sizeof(*mpActiveControlPoint) * (mpActiveControlPoint - &mControlPoints.front()),
                                         sizeof(*mpActiveControlPoint),
                                         *mpActiveControlPoint);
         mVBOControlPoints.Unbind();
      }

      break;

   case WM_LBUTTONDOWN:
   {
      // get the current screen x and y location
      const auto x = static_cast< size_t >(lParam & 0xFFFF);
      const auto y = GetSize().height - static_cast< size_t >(lParam >> 16);

      // read the contents of the selection buffer...
      uint32_t selection = 0;
      mFBOCanvas.Bind(GL_READ_FRAMEBUFFER);
      mFBOCanvas.Read(x, y, 1, 1, GL_COLOR_ATTACHMENT1, GL_RED_INTEGER, GL_UNSIGNED_INT, &selection);
      mFBOCanvas.Unbind();

      // if the selection is not equal to the max, then a point was hit
      if (selection != std::numeric_limits< GLuint >::max())
      {
         // obtain the new hit location
         mpActiveControlPoint = &mControlPoints[selection];
      }
   }

   break;

   case WM_LBUTTONUP:
      // release the active point...
      mpActiveControlPoint = nullptr;

      break;

   case WM_CHAR:
      // evalutate the character being pressed...
      switch (wParam)
      {
      case '3':
      case '4':
      case '5':
      case '6':
      {
         // determine the number of control points to see
         const uint32_t number_of_control_points = std::stoul(std::string(1, static_cast< char >(wParam)));

         // update the number of points based on the pressed key
         mVBOControlPointsIndices.DeleteBuffer();
         mVBOControlPointsIndices.GenBuffer(GL_ELEMENT_ARRAY_BUFFER);
         mVBOControlPointsIndices.Bind();
         const uint32_t indices[] = { 0, 1, 2, 3, 4, 5 };
         mVBOControlPointsIndices.BufferStorage(sizeof(*indices) * number_of_control_points, indices, 0);
         mVBOControlPointsIndices.Unbind();

         // let the shader have the initial location
         mGenCurveShader.Enable();
         mGenCurveShader.SetUniformValue("number_of_control_points", number_of_control_points);
         mGenCurveShader.Disable();


         break;
      }

      default: result = OpenGLWindow::MessageHandler(uMsg, wParam, lParam); break;
      }

      break;

   default:
      // default handle the messages
      result = OpenGLWindow::MessageHandler(uMsg, wParam, lParam);
   }

   return result;
}
