#ifndef _VERTEX_BUFFER_OBJECT_H_
#define _VERTEX_BUFFER_OBJECT_H_

// local includes
#include "WglAssert.h"

// platform includes
#include "Window.h"

// gl includes
#include "GL/glew.h"
#include <GL/GL.h>

// std includes
#include <stack>
#include <cstdint>

class VertexBufferObject
{
public:
   // static function that gets the currently bound vbo
   static GLuint GetCurrentVBO( const GLenum type );

   // constructor / destructor
    VertexBufferObject( );
   ~VertexBufferObject( );

   // allow for move operations
   VertexBufferObject( VertexBufferObject && vao );
   VertexBufferObject & operator = ( VertexBufferObject && vao );

   // returns the vao id
   GLuint Handle( ) const { return mVBO; }
   operator GLuint ( ) const { return Handle(); }

   // generate / delete buffer
   void GenBuffer( const GLenum type );
   void DeleteBuffer( );

   // bind / unbind the vbo
   void Bind( );
   void Bind( const GLenum type );
   void Unbind( );

   // binds a buffer object to an indexed buffer target
   void BindBufferBase( const GLuint index );
   void UnbindBufferBase( const GLuint index );
   void BindBufferRange( const GLuint index,
                         const GLintptr offset,
                         const GLsizeiptr size );

   // indicates if currently bound
   bool IsBound( ) const { return mBound; }

   // introduce data to the buffer
   void BufferData( const GLsizeiptr size, const GLvoid * const pData, const GLenum usage );
   void BufferSubData( const GLintptr offset, const GLsizeiptr size, const GLvoid * const pData );

   // allocate a data store (buffer is immutable)
   // first signature buffer must be bound
   // second signature buffer does not need to be bound
   void BufferStorage( const GLsizeiptr size,
                       const void * const pData,
                       const GLbitfield flags );
   void NamedBufferStorage( const GLsizeiptr size,
                            const void * const pData,
                            const GLbitfield flags );

   // obtain raw pointer to gl memory
   uint8_t * MapBuffer( const GLenum access );
   void UnmapBuffer( );

   // gets the size of the buffered data
   size_t Size( ) const;
   template < typename T > size_t Size( ) const;

   // sets up vertex attributes
   void VertexAttribPointer( const GLuint index,
                             const GLint size,
                             const GLenum type,
                             const GLboolean normalized,
                             const GLsizei stride,
                             const GLuint offset );

private:
   // prohibit certain actions
   VertexBufferObject( const VertexBufferObject & );
   VertexBufferObject & operator = ( const VertexBufferObject & );

   // identifies the vbo
   GLuint      mVBO;

   // indicates the type of buffer target
   GLenum      mType;

   // indicates stored previous types from when
   // a bind operation needs to temporarily convert
   // to a different type for use... as an example,
   // transform feedback to an ordinary array buffer...
   typedef std::stack< GLenum > TypesStack;
   TypesStack  mPreviousTypes;

   // indicates the size of the buffer data in bytes
   size_t      mSize;

   // indicates if the vbo is bound
   bool        mBound;

};

inline size_t VertexBufferObject::Size( ) const
{
   return mSize;
}

template < typename T >
inline size_t VertexBufferObject::Size( ) const
{
   WGL_ASSERT(Size() % sizeof(T) == 0);

   return Size() / sizeof(T);
}

typedef VertexBufferObject VBO;

#endif // _VERTEX_BUFFER_OBJECT_H_
