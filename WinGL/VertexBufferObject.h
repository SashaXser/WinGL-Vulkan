#ifndef _VERTEX_BUFFER_OBJECT_H_
#define _VERTEX_BUFFER_OBJECT_H_

// platform includes
#include "Window.h"

// gl includes
#include "GL/glew.h"
#include <GL/GL.h>

// std includes
#include <cstdint>

class VertexBufferObject
{
public:
   // static function that gets the currently bound vbo
   static GLuint GetCurrentVBO( );

   // constructor / destructor
    VertexBufferObject( const GLenum type, const bool auto_create = false );
   ~VertexBufferObject( );

   // allow for move operations
   VertexBufferObject( VertexBufferObject && vao );
   VertexBufferObject & operator = ( VertexBufferObject && vao );

   // returns the vao id
   GLuint Handle( ) const { return mVBO; }
   operator GLuint ( ) const { Handle(); }

   // generate / delete buffer
   void GenBuffer( );
   void DeleteBuffer( );

   // bind / unbind the vbo
   void Bind( );
   void Unbind( );

   // indicates if currently bound
   bool IsBound( ) const { return mBound; }

   // introduce data to the buffer
   void BufferData( const GLsizeiptr size, const GLvoid * const pData, const GLenum usage );
   void BufferSubData( const GLintptr offset, const GLsizeiptr size, const GLvoid * const pData );

   // obtain raw pointer to gl memory
   uint8_t * MapBuffer( const GLenum access );
   void UnmapBuffer( );

   // sets up vertex attributes
   void VertexAttribPointer( const GLuint index,
                             const GLint size,
                             const GLenum type,
                             const GLboolean normalized,
                             const GLsizei stride,
                             const GLuint offset );

private:
   // prohibit certain actions
   VertexBufferObject( );
   VertexBufferObject( const VertexBufferObject & );
   VertexBufferObject & operator = ( const VertexBufferObject & );

   // identifies the vbo
   GLuint   mVBO;

   // indicates the type of buffer target
   GLenum   mType;

   // indicates if the vbo is bound
   bool     mBound;

};

#endif // _VERTEX_BUFFER_OBJECT_H_
