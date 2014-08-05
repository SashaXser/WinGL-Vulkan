// local includes
#include "VertexBufferObject.h"
#include "WglAssert.h"

// std includes
#include <algorithm>

GLuint VertexBufferObject::GetCurrentVBO( )
{
   GLint vbo = 0;
   glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &vbo);

   return static_cast< GLuint >(vbo);
}

VertexBufferObject::VertexBufferObject( const GLenum type, const bool auto_create ) :
mVBO     ( 0 ),
mType    ( type ),
mBound   ( false )
{

   if (auto_create) GenBuffer();
}

VertexBufferObject::~VertexBufferObject( )
{
   DeleteBuffer();
}

VertexBufferObject::VertexBufferObject( VertexBufferObject && vbo ) :
mVBO     ( 0 ),
mType    ( GL_ARRAY_BUFFER ),
mBound   ( false )
{
   std::swap(mVBO, vbo.mVBO);
   std::swap(mType, vbo.mType);
   std::swap(mBound, vbo.mBound);
}

VertexBufferObject & VertexBufferObject::operator = ( VertexBufferObject && vbo )
{
   if (this != &vbo)
   {
      std::swap(mVBO, vbo.mVBO);
      std::swap(mType, vbo.mType);
      std::swap(mBound, vbo.mBound);
   }

   return *this;
}

void VertexBufferObject::GenBuffer( )
{
   WGL_ASSERT(!mVBO);

   glGenBuffers(1, &mVBO);
}

void VertexBufferObject::DeleteBuffer( )
{
   if (mVBO)
   {
      WGL_ASSERT((mBound && VertexBufferObject::GetCurrentVBO() == mVBO) ||
                 (!mBound && VertexBufferObject::GetCurrentVBO() != mVBO));

      // if the vbo is bound, unbind it...
      if (mBound) glBindBuffer(mType, 0);

      // release the vertex buffer...
      glDeleteBuffers(1, &mVBO);
   }

   mVBO = 0;
   mBound = false;
}

void VertexBufferObject::Bind( )
{
   glBindBuffer(mType, mVBO); mBound = true;
}

void VertexBufferObject::Unbind( )
{
   glBindBuffer(mType, 0); mBound = false;
}

void VertexBufferObject::BufferData( const GLsizeiptr size, const GLvoid * const pData, const GLenum usage )
{
   WGL_ASSERT(mBound && VertexBufferObject::GetCurrentVBO() == mVBO);

   glBufferData(mType, size, pData, usage);
}

void VertexBufferObject::BufferSubData( const GLintptr offset, const GLsizeiptr size, const GLvoid * const pData )
{
   WGL_ASSERT(mBound && VertexBufferObject::GetCurrentVBO() == mVBO);

   glBufferSubData(mType, offset, size, pData);
}

uint8_t * VertexBufferObject::MapBuffer( const GLenum access )
{
   WGL_ASSERT(mBound && VertexBufferObject::GetCurrentVBO() == mVBO);

   return static_cast< uint8_t * >(glMapBuffer(mType, access));
}

void VertexBufferObject::UnmapBuffer( )
{
   WGL_ASSERT(mBound && VertexBufferObject::GetCurrentVBO() == mVBO);

   glUnmapBuffer(mType);
}

void VertexBufferObject::VertexAttribPointer( const GLuint index,
                                              const GLint size,
                                              const GLenum type,
                                              const GLboolean normalized,
                                              const GLsizei stride,
                                              const GLuint offset )
{
   WGL_ASSERT(mBound && VertexBufferObject::GetCurrentVBO() == mVBO);

   glVertexAttribPointer(index, size, type, normalized, stride, reinterpret_cast< GLvoid * >(offset));
}

