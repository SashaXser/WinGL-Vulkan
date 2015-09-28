// local includes
#include "QueryObject.h"
#include "WglAssert.h"

// platform includes
#include <windows.h>

// std includes
#include <utility>

QueryObject::QueryObject( ) :
mQueryType     ( 0 ),
mQueryID       ( 0 )
{
}

QueryObject::~QueryObject( )
{
   if (mQueryID)
   {
      // must happen within a valid gl context
      WGL_ASSERT(wglGetCurrentContext());

      // release the query object
      DeleteQuery();
   }
}

QueryObject::QueryObject( QueryObject && query ) :
QueryObject()
{
   std::swap(mQueryID, query.mQueryID);
   std::swap(mQueryType, query.mQueryType);
}

QueryObject & QueryObject::operator = ( QueryObject && query )
{
   std::swap(mQueryID, query.mQueryID);
   std::swap(mQueryType, query.mQueryType);

   return *this;
}

void QueryObject::GenQuery( const GLenum type )
{
   WGL_ASSERT(!mQueryID);

   mQueryType = type;

   glGenQueries(1, &mQueryID);
}

void QueryObject::DeleteQuery( )
{
   if (mQueryID)
   {
      mQueryType = 0;

      glDeleteQueries(1, &mQueryID);
   }
}

void QueryObject::Begin( )
{
   WGL_ASSERT(mQueryID);

   glBeginQuery(mQueryType, mQueryID);
}

void QueryObject::End( )
{
   WGL_ASSERT(mQueryID);

   glEndQuery(mQueryType);
}

template < typename T > struct GetQueryObject;
template < > struct GetQueryObject< GLint > { static GLint Value( const GLuint id, const GLenum name ) { GLint value = 0; glGetQueryObjectiv(id, name, &value); return value; } };
template < > struct GetQueryObject< GLuint > { static GLuint Value( const GLuint id, const GLenum name ) { GLuint value = 0; glGetQueryObjectuiv(id, name, &value); return value; } };
template < > struct GetQueryObject< GLint64 > { static GLint64 Value( const GLuint id, const GLenum name ) { GLint64 value = 0; glGetQueryObjecti64v(id, name, &value); return value; } };
template < > struct GetQueryObject< GLuint64 > { static GLuint64 Value( const GLuint id, const GLenum name ) { GLuint64 value = 0; glGetQueryObjectui64v(id, name, &value); return value; } };

bool QueryObject::ResultReady( ) const
{
   WGL_ASSERT(mQueryID);

   const auto available = GetQueryObject< GLuint >::Value(mQueryID, GL_QUERY_RESULT_AVAILABLE);

   return available == GL_TRUE;
}

template < typename T >
T QueryObject::Value( const bool wait ) const
{
   WGL_ASSERT(mQueryID);
   
   const GLint query_result_bit_size = [ this ] ( )
   {
      GLint bit_size = 0;
      glGetQueryiv(mQueryType, GL_QUERY_COUNTER_BITS, &bit_size);

      return bit_size;
   }();

   WGL_ASSERT(query_result_bit_size >= sizeof(T) * 8);

   return query_result_bit_size >= sizeof(T) * 8 ?
          GetQueryObject< T >::Value(mQueryID, wait ? GL_QUERY_RESULT_NO_WAIT : GL_QUERY_RESULT) :
          0;
}

template GLint QueryObject::Value< GLint >( const bool wait ) const;
template GLuint QueryObject::Value< GLuint >( const bool wait ) const;
template GLint64 QueryObject::Value< GLint64 >( const bool wait ) const;
template GLuint64 QueryObject::Value< GLuint64 >( const bool wait ) const;
