#ifndef _QUERY_OBJECT_H_
#define _QUERY_OBJECT_H_

// gl includes
#include <GL/glew.h>
#include <GL/GL.h>

class QueryObject
{
public:
   // constructor / destructor
    QueryObject( );
   ~QueryObject( );

   // only allow move construction and assignment
   QueryObject( QueryObject && query );
   QueryObject & operator = ( QueryObject && query );

   // generates the query object
   void GenQuery( const GLenum type );
   void DeleteQuery( );

   // indicates the type of query
   GLenum Type( ) const { return mQueryType; }

   // start and stop the query operation
   void Begin( );
   void End( );

   // determines if a result is ready
   bool ResultReady( ) const;

   // request the result of the query (stalls if not ready)
   template < typename T > T Value( const bool wait = true ) const;

private:
   // defines the type of query
   GLenum   mQueryType;

   // defines the query object id
   GLuint   mQueryID;

};

#endif // _QUERY_OBJECT_H_
