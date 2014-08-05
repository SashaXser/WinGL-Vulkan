#ifndef _VERTEX_ARRAY_OBJECT_H_
#define _VERTEX_ARRAY_OBJECT_H_

// platform includes
#include "Window.h"

// gl includes
#include "GL/glew.h"
#include <GL/GL.h>

class VertexArrayObject
{
public:
   // static function that gets the currently bound vao
   static GLuint GetCurrentVAO( );

   // constructor / destructor
    VertexArrayObject( const bool auto_create = false );
   ~VertexArrayObject( );

   // allow for move operations
   VertexArrayObject( VertexArrayObject && vao );
   VertexArrayObject & operator = ( VertexArrayObject && vao );

   // returns the vao id
   GLuint Handle( ) const { return mVAO; }
   operator GLuint ( ) const { Handle(); }

   // generate / delete array
   void GenArray( );
   void DeleteArray( );

   // bind / unbind the vao
   void Bind( );
   void Unbind( );

   // indicates if currently bound
   bool IsBound( ) const { return mBound; }

   // enables / disable the vertex array attributes
   void EnableVertexAttribArray( const GLuint index );
   void DisableVertexAttribArray( const GLuint index );

private:
   // prohibit certain actions
   VertexArrayObject( const VertexArrayObject & );
   VertexArrayObject & operator = ( const VertexArrayObject & );

   // identifies the vao
   GLuint   mVAO;

   // indicates if the vao is bound
   bool     mBound;

};

#endif // _VERTEX_ARRAY_OBJECT_H_
