// local includes
#include "Pipeline.h"
#include "WglAssert.h"
#include "TransformFeedbackObject.h"

// std includes
#include <cassert>

Pipeline::Pipeline( ) :
mRasterDiscardEnabled      ( false ),
mCullFaceEnabled           ( false ),
mDepthTestingEnabled       ( false ),
mProgramPointSizeEnabled   ( false ),
mBlendingEnabled           ( false ),
mPolygonMode               ( GL_FILL ),
mEnabledReadBuffer         ( GL_BACK_LEFT ),
mEnabledDrawBuffer         ( GL_BACK_LEFT ),
mEnabledDrawBuffers        ( { GL_BACK_LEFT } ),
mTransformFeedbackEnabled  ( false ),
mTransformFeedbackMode     ( GL_POINTS )
{
}

Pipeline::~Pipeline( )
{
}

void Pipeline::EnableRasterDiscard( const bool enable )
{
   if (enable)
   {
      glEnable(GL_RASTERIZER_DISCARD);
   }
   else
   {
      glDisable(GL_RASTERIZER_DISCARD);
   }

   mRasterDiscardEnabled = enable;
}

void Pipeline::EnableCullFace( const bool enable )
{
   if (enable)
   {
      glEnable(GL_CULL_FACE);
   }
   else
   {
      glDisable(GL_CULL_FACE);
   }

   mCullFaceEnabled = enable;
}

void Pipeline::EnableDepthTesting( const bool enable )
{
   if (enable)
   {
      glEnable(GL_DEPTH_TEST);
   }
   else
   {
      glDisable(GL_DEPTH_TEST);
   }

   mDepthTestingEnabled = enable;
}

void Pipeline::EnableProgramPointSize( const bool enable )
{
   if (enable)
   {
      glEnable(GL_PROGRAM_POINT_SIZE);
   }
   else
   {
      glDisable(GL_PROGRAM_POINT_SIZE);
   }

   mProgramPointSizeEnabled = enable;
}

void Pipeline::SetPolygonMode( const GLenum mode )
{
   assert(mode == GL_POINT ||
          mode == GL_LINE ||
          mode == GL_FILL);

   glPolygonMode(GL_FRONT_AND_BACK, mode);

   mPolygonMode = mode;
}

void Pipeline::DrawBuffers( const std::vector< GLenum > & buffers )
{
   glDrawBuffers(static_cast< GLsizei >(buffers.size()), &buffers.front());

   mEnabledDrawBuffers = buffers;
}

void Pipeline::DrawBuffers( const GLenum * const pBuffers, const size_t count )
{
   DrawBuffers(std::vector< GLenum >(pBuffers, pBuffers + count));
}

void Pipeline::DrawBuffer( const GLenum buffer )
{
   glDrawBuffer(buffer);

   mEnabledDrawBuffer = buffer;
}

void Pipeline::ReadBuffer( const GLenum buffer )
{
   glReadBuffer(buffer);

   mEnabledReadBuffer = buffer;
}

template < typename T > struct clear_buffer_selector;
template < > struct clear_buffer_selector< GLint > { const decltype(glClearBufferiv) ClearBuffer { glClearBufferiv }; };
template < > struct clear_buffer_selector< GLuint > { const decltype(glClearBufferuiv) ClearBuffer { glClearBufferuiv }; };
template < > struct clear_buffer_selector< GLfloat > { const decltype(glClearBufferfv) ClearBuffer { glClearBufferfv }; };

template< typename T >
void Pipeline::ClearBuffer( const GLenum buffer,
                            const GLint draw_buffer,
                            const T * const pValue ) const
{
   clear_buffer_selector< T >().ClearBuffer(buffer, draw_buffer, pValue);
}

template void Pipeline::ClearBuffer< >( const GLenum buffer, GLint draw_buffer, const GLint * const pValue ) const;
template void Pipeline::ClearBuffer< >( const GLenum buffer, GLint draw_buffer, const GLuint * const pValue ) const;
template void Pipeline::ClearBuffer< >( const GLenum buffer, GLint draw_buffer, const GLfloat * const pValue ) const;

void Pipeline::EnableTransformFeedback( const bool enable, const GLenum mode )
{
   if (enable)
   {
      glBeginTransformFeedback(mode);
   }
   else
   {
      glEndTransformFeedback();
   }

   mTransformFeedbackMode = mode;
   mTransformFeedbackEnabled = enable;
}

#include <algorithm>

void Pipeline::EnableVertexAttribArray( const bool enable, const size_t attribute )
{
   if (enable)
   {
      glEnableVertexAttribArray(static_cast< GLuint >(attribute));
      
      mEnabledVertAttribArray.resize(std::max(mEnabledVertAttribArray.size(), attribute + 1));

      mEnabledVertAttribArray[attribute] = false;
   }
   else
   {
      glDisableVertexAttribArray(static_cast< GLuint >(attribute));

      mEnabledVertAttribArray[attribute] = false;
   }
}

bool Pipeline::IsVertexAttribArrayEnabled( const size_t attribute )
{
   return mEnabledVertAttribArray.size() > attribute ? mEnabledVertAttribArray[attribute] : false;
}

void Pipeline::DrawArrays( const GLenum mode, const size_t first, const size_t count ) const
{
   glDrawArrays(mode, static_cast< GLint >(first), static_cast< GLsizei >(count));
}

void Pipeline::DrawElements( const GLenum mode, const size_t count, const GLenum type, const void * const pIndices ) const
{
   glDrawElements(mode, static_cast< GLsizei >(count), type, pIndices);
}

void Pipeline::DrawTransformFeedback( const GLenum mode, const TransformFeedbackObject & tfo ) const
{
   // requires one to have been created
   WGL_ASSERT(tfo);

   glDrawTransformFeedback(mode, tfo);
}

void Pipeline::SetViewport( const GLint x, const GLint y,
                            const GLint width, const GLint height )
{
   if (mViewportStack.empty())
   {
      PushViewport(x, y, width, height);
   }
   else
   {
      mViewportStack.back() = { x, y, width, height };

      glViewport(x, y, width, height);
   }
}

void Pipeline::PushViewport( const GLint x, const GLint y,
                             const GLint width, const GLint height )
{
   mViewportStack.emplace_back(Viewport { x, y, width, height });

   glViewport(x, y, width, height);
}

void Pipeline::PopViewport( )
{
   assert(mViewportStack.size() > 1);

   mViewportStack.pop_back();

   const Viewport & vp = mViewportStack.back();

   glViewport(vp.x, vp.y, vp.width, vp.height);
}

void Pipeline::EnableStandardBlending( const bool enable )
{
   if (mBlendingEnabled = enable)
   {
      glEnable(GL_BLEND);

      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   }
   else
   {
      glDisable(GL_BLEND);
   }
}
