// local includes
#include "TransformFeedbackObject.h"
#include "WglAssert.h"

// platform includes
#include <Windows.h>

// std includes
#include <utility>

GLuint TransformFeedbackObject::GetCurrentTFO( )
{
   GLint tfo = 0;
   glGetIntegerv(GL_TRANSFORM_FEEDBACK_BUFFER_BINDING, &tfo);

   return static_cast< GLuint >(tfo);
}

TransformFeedbackObject::TransformFeedbackObject( ) :
mTFO     ( 0 ),
mBound   ( false )
{
}

TransformFeedbackObject::~TransformFeedbackObject( )
{
   // validate some assumptions
   WGL_ASSERT(!mBound);
   WGL_ASSERT(wglGetCurrentContext());

   // release any resources
   if (mTFO) glDeleteTransformFeedbacks(1, &mTFO);
}

TransformFeedbackObject::TransformFeedbackObject( TransformFeedbackObject && tfo )
{
   // just swap the two values
   *this = std::move(tfo);
}

TransformFeedbackObject & TransformFeedbackObject::operator = ( TransformFeedbackObject && tfo )
{
   if (this != &tfo)
   {
      std::swap(mTFO, tfo.mTFO);
      std::swap(mBound, tfo.mBound);
   }

   return *this;
}

void TransformFeedbackObject::GenBuffer( )
{
   // validate some assumptions
   WGL_ASSERT(!mTFO);
   WGL_ASSERT(!mBound);
   WGL_ASSERT(wglGetCurrentContext());

   // generate a single transform feedback object
   glGenTransformFeedbacks(1, &mTFO);
}

void TransformFeedbackObject::DeleteBuffer( )
{
   // validate some assumptions
   WGL_ASSERT(mTFO);
   WGL_ASSERT(!mBound);
   WGL_ASSERT(wglGetCurrentContext());

   // just move an empty tfo into this object
   *this = TransformFeedbackObject();
}

void TransformFeedbackObject::Bind( )
{
   // validate some assumptions
   WGL_ASSERT(mTFO);
   WGL_ASSERT(!mBound);
   WGL_ASSERT(wglGetCurrentContext());

   // bind the transform feedback object
   glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, mTFO);

   // transform feedback now bound
   mBound = true;
}

void TransformFeedbackObject::Unbind( )
{
   // validate some assumptions
   WGL_ASSERT(mTFO);
   WGL_ASSERT(mBound);
   WGL_ASSERT(wglGetCurrentContext());

   // unbind the transform feedback object
   glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

   // transform feedback now not bound
   mBound = false;
}
