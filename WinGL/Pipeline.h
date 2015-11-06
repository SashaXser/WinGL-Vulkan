#ifndef _PIPELINE_H_
#define _PIPELINE_H_

// std includes
#include <vector>
#include <cstdint>

// gl includes
#include <GL/glew.h>
#include <GL/GL.h>

// forward declarations
class TransformFeedbackObject;

class Pipeline
{
public:
   // constructor / destructor
   Pipeline( );
   ~Pipeline( );

   // enables / disables raster discard
   void EnableRasterDiscard( const bool enable );
   bool IsRasterDiscardEnabled( ) const { return mRasterDiscardEnabled; }

   // enables / disables testing for face removal
   void EnableCullFace( const bool enable );
   bool IsCullFaceEnabled( ) const { return mCullFaceEnabled; }

   // enables / disables depth testing
   void EnableDepthTesting( const bool enable );
   bool IsDepthTestingEnabled( ) const { return mDepthTestingEnabled; }

   // enables / disables shader point size control
   void EnableProgramPointSize( const bool enable );
   bool IsProgramPointSizeEnabled( ) const { return mProgramPointSizeEnabled; }

   // enables buffers to be drawn into
   void DrawBuffers( const std::vector< GLenum > & buffers );
   void DrawBuffers( const GLenum * const pBuffers, const size_t count );
   const std::vector< GLenum > & EnabledDrawBuffers( ) const { return mEnabledDrawBuffers; }

   // enables buffer to be read from
   void ReadBuffer( const GLenum buffer );
   GLenum EnabledReadBuffer( ) const { return mEnabledReadBuffer; }

   // clear a specified buffer
   template < typename T >
   void ClearBuffer( const GLenum buffer,
                     const GLint draw_buffer,
                     const T * const pValue ) const;

   // enables / disables transform feedback
   void EnableTransformFeedback( const bool enable, const GLenum mode = GL_POINTS );
   bool IsTransformFeedbackEnabled( ) const { return mTransformFeedbackEnabled; }

   // enables / disables a vertex attribute for stream processing
   void EnableVertexAttribArray( const bool enable, const size_t attribute );
   bool IsVertexAttribArrayEnabled( const size_t attribute );

   // render a set of vertices from a linear array
   void DrawArrays( const GLenum mode, const size_t first, const size_t count ) const;

   // render a set of vertices from a index array
   void DrawElements( const GLenum mode, const size_t count, const GLenum type, const void * const pIndices ) const;

   // renders a set of vertices from a transform feedback object
   void DrawTransformFeedback( const GLenum mode, const TransformFeedbackObject & tfo ) const;

private:
   // private member variables
   bool mRasterDiscardEnabled;
   bool mCullFaceEnabled;
   bool mDepthTestingEnabled;
   bool mProgramPointSizeEnabled;

   std::vector< bool > mEnabledVertAttribArray;

   GLenum   mEnabledReadBuffer;
   std::vector< GLenum > mEnabledDrawBuffers;

   bool mTransformFeedbackEnabled;
   GLenum mTransformFeedbackMode;

};

#endif // _PIPELINE_H_
