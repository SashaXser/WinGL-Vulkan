// local includes
#include "VertexBufferObject.h"

// std includes
#include <algorithm>

GLuint VertexBufferObject::GetCurrentVBO( const GLenum type )
{
   GLint vbo = 0;

   switch (type)
   {
   case GL_ARRAY_BUFFER:              glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &vbo);              break;
   case GL_ELEMENT_ARRAY_BUFFER:      glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &vbo);      break;
   case GL_TRANSFORM_FEEDBACK_BUFFER: glGetIntegerv(GL_TRANSFORM_FEEDBACK_BUFFER_BINDING, &vbo); break;
   default: WGL_ASSERT(!"Need To Fill In This VBO Type!!!"); break;
   }

   return static_cast< GLuint >(vbo);
}

VertexBufferObject::VertexBufferObject( ) :
mVBO     ( 0 ),
mType    ( GL_ARRAY_BUFFER ),
mSize    ( 0 ),
mBound   ( false )
{
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

void VertexBufferObject::GenBuffer( const GLenum type )
{
   WGL_ASSERT(!mVBO);

   mType = type;

   glGenBuffers(1, &mVBO);
}

void VertexBufferObject::DeleteBuffer( )
{
   if (mVBO)
   {
      // must happen within a valid gl context
      WGL_ASSERT(wglGetCurrentContext());

      WGL_ASSERT((mBound && VertexBufferObject::GetCurrentVBO(mType) == mVBO) ||
                 (!mBound && VertexBufferObject::GetCurrentVBO(mType) != mVBO));

      // if the vbo is bound, unbind it...
      if (mBound) glBindBuffer(mType, 0);

      // release the vertex buffer...
      glDeleteBuffers(1, &mVBO);

      // clear the contents of previous types
      mPreviousTypes = TypesStack();
   }

   mVBO = 0;
   mBound = false;
}

void VertexBufferObject::Bind( )
{
   WGL_ASSERT(mVBO);
   WGL_ASSERT(mType);
   WGL_ASSERT(!mBound);

   glBindBuffer(mType, mVBO); mBound = true;
}

void VertexBufferObject::Bind( const GLenum type )
{
   WGL_ASSERT(mVBO);
   WGL_ASSERT(mType);
   WGL_ASSERT(mPreviousTypes.size() != 10);

   // save the previous type
   mPreviousTypes.push(mType);

   // save the new type for binding
   mType = type;

   // perform the bind
   Bind();
}

void VertexBufferObject::Unbind( )
{
   WGL_ASSERT(mVBO);
   WGL_ASSERT(mType);
   WGL_ASSERT(mBound);

   glBindBuffer(mType, 0); mBound = false;

   // restore the previous type if not empty
   if (!mPreviousTypes.empty())
   {
      mType = mPreviousTypes.top(); mPreviousTypes.pop();
   }
}

void VertexBufferObject::BindBufferBase( const GLuint index )
{
   WGL_ASSERT(mBound && VertexBufferObject::GetCurrentVBO(mType) == mVBO);
   WGL_ASSERT(mType == GL_ATOMIC_COUNTER_BUFFER ||
              mType == GL_TRANSFORM_FEEDBACK_BUFFER ||
              mType == GL_UNIFORM_BUFFER ||
              mType == GL_SHADER_STORAGE_BUFFER);

   glBindBufferBase(mType, index, mVBO);
}

void VertexBufferObject::UnbindBufferBase( const GLuint index )
{
   WGL_ASSERT(mType == GL_ATOMIC_COUNTER_BUFFER ||
              mType == GL_TRANSFORM_FEEDBACK_BUFFER ||
              mType == GL_UNIFORM_BUFFER ||
              mType == GL_SHADER_STORAGE_BUFFER);

   glBindBufferBase(mType, index, 0);
}

void VertexBufferObject::BindBufferRange( const GLuint index,
                                          const GLintptr offset,
                                          const GLsizeiptr size )
{
   WGL_ASSERT(mBound && VertexBufferObject::GetCurrentVBO(mType) == mVBO);
   WGL_ASSERT(mType == GL_ATOMIC_COUNTER_BUFFER ||
              mType == GL_TRANSFORM_FEEDBACK_BUFFER ||
              mType == GL_UNIFORM_BUFFER ||
              mType == GL_SHADER_STORAGE_BUFFER);

   glBindBufferRange(mType, index, mVBO, offset, size);
}

void VertexBufferObject::BufferData( const GLsizeiptr size, const GLvoid * const pData, const GLenum usage )
{
   WGL_ASSERT(mBound && VertexBufferObject::GetCurrentVBO(mType) == mVBO);

   glBufferData(mType, size, pData, usage);

   mSize = size;
}

void VertexBufferObject::BufferSubData( const GLintptr offset, const GLsizeiptr size, const GLvoid * const pData )
{
   WGL_ASSERT(mBound && VertexBufferObject::GetCurrentVBO(mType) == mVBO);

   glBufferSubData(mType, offset, size, pData);
}

void VertexBufferObject::BufferStorage( const GLsizeiptr size,
                                        const void * const pData,
                                        const GLbitfield flags )
{
   WGL_ASSERT(mBound && VertexBufferObject::GetCurrentVBO(mType) == mVBO);

   glBufferStorage(mType, size, pData, flags);

   mSize = size;
}

void VertexBufferObject::NamedBufferStorage( const GLsizeiptr size,
                                             const void * const pData,
                                             const GLbitfield flags )
{
   glNamedBufferStorage(mVBO, size, pData, flags);

   mSize = size;
}

uint8_t * VertexBufferObject::MapBuffer( const GLenum access )
{
   WGL_ASSERT(mBound && VertexBufferObject::GetCurrentVBO(mType) == mVBO);

   return static_cast< uint8_t * >(glMapBuffer(mType, access));
}

void VertexBufferObject::UnmapBuffer( )
{
   WGL_ASSERT(mBound && VertexBufferObject::GetCurrentVBO(mType) == mVBO);

   glUnmapBuffer(mType);
}

void VertexBufferObject::VertexAttribPointer( const GLuint index,
                                              const GLint size,
                                              const GLenum type,
                                              const GLboolean normalized,
                                              const GLsizei stride,
                                              const GLuint offset )
{
   WGL_ASSERT(mBound && VertexBufferObject::GetCurrentVBO(mType) == mVBO);

   glVertexAttribPointer(index, size, type, normalized, stride, reinterpret_cast< GLvoid * >(offset));
}

