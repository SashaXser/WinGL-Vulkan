// local includes
#include "VertexArrayObject.h"
#include "WglAssert.h"

// std includes
#include <algorithm>

GLuint VertexArrayObject::GetCurrentVAO( )
{
   GLint vao = 0;
   glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &vao);

   return static_cast< GLuint >(vao);
}

VertexArrayObject::VertexArrayObject( ) :
mVAO     ( 0 ),
mBound   ( false )
{
}

VertexArrayObject::~VertexArrayObject( )
{
   DeleteArray();
}

VertexArrayObject::VertexArrayObject( VertexArrayObject && vao ) :
mVAO     ( 0 ),
mBound   ( false )
{
   std::swap(mVAO, vao.mVAO);
   std::swap(mBound, vao.mBound);
}

VertexArrayObject & VertexArrayObject::operator = ( VertexArrayObject && vao )
{
   if (this != &vao)
   {
      std::swap(mVAO, vao.mVAO);
      std::swap(mBound, vao.mBound);
   }

   return *this;
}

void VertexArrayObject::GenArray( )
{
   WGL_ASSERT(!mVAO);

   glGenVertexArrays(1, &mVAO);
}

void VertexArrayObject::DeleteArray( )
{
   if (mVAO)
   {
      // must happen within a valid gl context
      WGL_ASSERT(wglGetCurrentContext());

      WGL_ASSERT((mBound && VertexArrayObject::GetCurrentVAO() == mVAO) ||
                 (!mBound && VertexArrayObject::GetCurrentVAO() != mVAO));

      // if the vao is bound, unbind it...
      if (mBound) glBindVertexArray(0);

      // release the vertex array...
      glDeleteVertexArrays(1, &mVAO);
   }

   mVAO = 0;
   mBound = false;
}

void VertexArrayObject::Bind( )
{
   glBindVertexArray(mVAO); mBound = true;
}

void VertexArrayObject::Unbind( )
{
   glBindVertexArray(0); mBound = false;
}

void VertexArrayObject::EnableVertexAttribArray( const GLuint index )
{
   WGL_ASSERT(mBound && VertexArrayObject::GetCurrentVAO() == mVAO);

   glEnableVertexAttribArray(index);
}

void VertexArrayObject::DisableVertexAttribArray( const GLuint index )
{
   WGL_ASSERT(mBound && VertexArrayObject::GetCurrentVAO() == mVAO);

   glDisableVertexAttribArray(index);
}

void VertexArrayObject::EnableVertexArrayAttrib( const GLuint index )
{
   glEnableVertexArrayAttrib(mVAO, index);
}

void VertexArrayObject::DisableVertexArrayAttrib( const GLuint index )
{
   glDisableVertexArrayAttrib(mVAO, index);
}

void VertexArrayObject::VertexArrayVertexBuffer( const GLuint binding_index,
                                                 const GLuint buffer,
                                                 const GLintptr offset,
                                                 const GLsizei stride)
{
   glVertexArrayVertexBuffer(mVAO, binding_index, buffer, offset, stride);
}

void VertexArrayObject::VertexArrayAttribFormat( const GLuint attrib_index,
                                                 const GLint size,
                                                 const GLenum type,
                                                 const GLboolean normalized,
                                                 const GLuint relative_offset )
{
   glVertexArrayAttribFormat(mVAO, attrib_index, size, type, normalized, relative_offset);
}
