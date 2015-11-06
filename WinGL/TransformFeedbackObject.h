#ifndef _TRANSFORM_FEEDBACK_OBJECT_H_
#define _TRANSFORM_FEEDBACK_OBJECT_H_

// gl includes
#include "GL/glew.h"
#include <GL/GL.h>

class TransformFeedbackObject
{
public:
   // static function that gets the currently bound tfo
   static GLuint GetCurrentTFO( );

   // constructor / destructor
    TransformFeedbackObject( );
   ~TransformFeedbackObject( );

   // allow for move operations
   TransformFeedbackObject( TransformFeedbackObject && tfo );
   TransformFeedbackObject & operator = ( TransformFeedbackObject && tfo );

   // returns the vao id
   GLuint Handle( ) const { return mTFO; }
   operator GLuint ( ) const { return mTFO; }

   // generate / delete buffer
   void GenBuffer( );
   void DeleteBuffer( );

   // bind / unbind the vbo
   void Bind( );
   void Unbind( );

   // indicates if currently bound
   bool IsBound( ) const { return mBound; }

private:
   // prohibit certain actions
   TransformFeedbackObject( const TransformFeedbackObject & ) = delete;
   TransformFeedbackObject & operator = ( const TransformFeedbackObject & ) = delete;

   // identifies the vbo
   GLuint      mTFO;

   // indicates if the vbo is bound
   bool        mBound;

};

typedef TransformFeedbackObject TFO;

#endif // _TRANSFORM_FEEDBACK_OBJECT_H_
