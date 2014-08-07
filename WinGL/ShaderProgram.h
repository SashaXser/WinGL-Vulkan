#ifndef _SHADER_PROGRAM_H_
#define _SHADER_PROGRAM_H_

// local includes
#include "WglAssert.h"

// platform includes
#include "Window.h"

// gl includes
#include "GL/glew.h"
#include <GL/GL.h>

// std includes
#include <map>
#include <string>
#include <cstring>
#include <type_traits>

// defines an invalid uniform location
extern const GLint INVALID_UNIFORM_LOCATION;

class ShaderProgram
{
public:
   // static function that gets the current program
   static GLuint GetCurrentProgram( );

   // constructor / destructor
    ShaderProgram( );
   ~ShaderProgram( );

   // move operations
   ShaderProgram( ShaderProgram && program );
   ShaderProgram & operator = ( ShaderProgram && program );

   // returns the shader program
   GLuint Handle( ) const { return mShaderProg; }
   operator GLuint ( ) const { return Handle(); }

   // attaches a program to the shader
   bool Attach( const GLenum shader, const std::string & src );
   bool AttachFile( const GLenum shader, const std::string & file );

   // links all the attached shaders together
   bool Link( );

   // gets a uniforms location
   GLint GetUniformLocation( const std::string & uniform );

   // gets a uniforms value
   template < typename T > bool GetUniformValue( const GLint uniform, T & t ) const;
   template < typename T > bool GetUniformValue( const std::string & uniform, T & t );

   // sets a uniforms value
   template < typename T > bool SetUniformValue( const GLint uniform, const T & t1 );
   template < typename T > bool SetUniformValue( const GLint uniform, const T & t1, const T & t2 );
   template < typename T > bool SetUniformValue( const GLint uniform, const T & t1, const T & t2, const T & t3 );
   template < typename T > bool SetUniformValue( const GLint uniform, const T & t1, const T & t2, const T & t3, const T & t4 );
   template < typename T > bool SetUniformValue( const std::string & uniform, const T & t1 );
   template < typename T > bool SetUniformValue( const std::string & uniform, const T & t1, const T & t2 );
   template < typename T > bool SetUniformValue( const std::string & uniform, const T & t1, const T & t2, const T & t3 );
   template < typename T > bool SetUniformValue( const std::string & uniform, const T & t1, const T & t2, const T & t3, const T & t4 );

   // enables / disables the use of the shader
   void Enable( );
   void Disable( );

private:
   // private typedefs
   typedef std::map< const std::string, const GLint > UniformLocationCtr;

   // prohibit certain actions
   ShaderProgram( const ShaderProgram & );
   ShaderProgram & operator = ( const ShaderProgram & );

   // helper template structs to get uniform values
   template < typename T > struct UniformValueSelector;

   // program object
   GLuint   mShaderProg;

   // uniform attribute locations
   UniformLocationCtr   mUniformLocCtr;

};

template < > struct ShaderProgram::UniformValueSelector< GLfloat >
{
   typedef GLfloat type;

   static const type INVALID_VALUE;
   static bool IsValid( const type & value ) { return INVALID_VALUE != value; }
   static bool IsValid( const type * pValue ) { return INVALID_VALUE != *pValue; }

   static void GetValue( const GLuint prog, const GLint loc, type & value ) { glGetUniformfv(prog, loc, &value); }
   static void GetValue( const GLuint prog, const GLint loc, type * const pValue ) { glGetUniformfv(prog, loc, pValue); }

   template < bool IS_ARRAY, size_t SIZE > struct array_selector { };

   static void SetValue( const GLint loc, const type & v1, const array_selector< false, 4 > ) { glUniform1f(loc, v1); }
   static void SetValue( const GLint loc, const type & v1, const type & v2 ) { glUniform2f(loc, v1, v2); }
   static void SetValue( const GLint loc, const type & v1, const type & v2, const type & v3 ) { glUniform3f(loc, v1, v2, v3); }
   static void SetValue( const GLint loc, const type & v1, const type & v2, const type & v3, const type & v4 ) { glUniform4f(loc, v1, v2, v3, v4); }

   static void SetValue( const GLint loc, const type * const pValues, const array_selector< true, sizeof(type) * 1> ) { glUniform1f(loc, pValues[0]); }
   static void SetValue( const GLint loc, const type * const pValues, const array_selector< true, sizeof(type) * 2 > ) { glUniform2f(loc, pValues[0], pValues[1]); }
   static void SetValue( const GLint loc, const type * const pValues, const array_selector< true, sizeof(type) * 3 > ) { glUniform3f(loc, pValues[0], pValues[1], pValues[2]); }
   static void SetValue( const GLint loc, const type * const pValues, const array_selector< true, sizeof(type) * 4 > ) { glUniform4f(loc, pValues[0], pValues[1], pValues[2], pValues[3]); }
};

template < > struct ShaderProgram::UniformValueSelector< GLdouble >
{
   typedef GLdouble type;

   static const type INVALID_VALUE;
   static bool IsValid( const type & value ) { return INVALID_VALUE != value; }
   static bool IsValid( const type * pValue ) { return INVALID_VALUE != *pValue; }

   static void GetValue( const GLuint prog, const GLint loc, type & value ) { glGetUniformdv(prog, loc, &value); }
   static void GetValue( const GLuint prog, const GLint loc, type * pValue ) { glGetUniformdv(prog, loc, pValue); }
};

template < > struct ShaderProgram::UniformValueSelector< GLint >
{
   typedef GLint type;

   static const type INVALID_VALUE;
   static bool IsValid( const type & value ) { return INVALID_VALUE != value; }
   static bool IsValid( const type * pValue ) { return INVALID_VALUE != *pValue; }

   static void GetValue( const GLuint prog, const GLint loc, type & value ) { glGetUniformiv(prog, loc, &value); }
   static void GetValue( const GLuint prog, const GLint loc, type * pValue ) { glGetUniformiv(prog, loc, pValue); }
};

template < > struct ShaderProgram::UniformValueSelector< GLuint >
{
   typedef GLuint type;

   static const type INVALID_VALUE;
   static bool IsValid( const type & value ) { return INVALID_VALUE != value; }
   static bool IsValid( const type * pValue ) { return INVALID_VALUE != *pValue; }

   static void GetValue( const GLuint prog, const GLint loc, type & value ) { glGetUniformuiv(prog, loc, &value); }
   static void GetValue( const GLuint prog, const GLint loc, type * pValue ) { glGetUniformuiv(prog, loc, pValue); }
};

template < typename T >
inline bool ShaderProgram::GetUniformValue( const GLint uniform, T & t ) const
{
   WGL_ASSERT(mShaderProg);

   typedef std::remove_extent< T >::type Type;

   bool obtained = false;

   T value = { UniformValueSelector< Type >::INVALID_VALUE };

   UniformValueSelector< Type >::GetValue(mShaderProg, uniform, value);

   if (UniformValueSelector< Type >::IsValid(value))
   {
      std::memcpy(&t, &value, sizeof(t)); obtained = true;
   }

   return obtained;
}

template < typename T >
inline bool ShaderProgram::GetUniformValue( const std::string & uniform, T & t )
{
   bool obtained = false;

   const GLint uniform_loc = GetUniformLocation(uniform);

   if (INVALID_UNIFORM_LOCATION != uniform_loc)
   {
      obtained = GetUniformValue(uniform_loc, t);
   }

   return obtained;
}

template < typename T >
inline bool ShaderProgram::SetUniformValue( const GLint uniform, const T & t1 )
{
   WGL_ASSERT(mShaderProg && ShaderProgram::GetCurrentProgram() == mShaderProg);

   typedef std::remove_const< std::remove_extent< T >::type >::type Type;
   typedef UniformValueSelector< Type >::array_selector< std::is_array< T >::value, sizeof(t1) > array_selector;

   UniformValueSelector< Type >::SetValue(uniform, t1, array_selector());

   return true;
}

template < typename T >
inline bool ShaderProgram::SetUniformValue( const GLint uniform, const T & t1, const T & t2 )
{
   WGL_ASSERT(mShaderProg && ShaderProgram::GetCurrentProgram() == mShaderProg);

   UniformValueSelector< T >::SetValue(uniform, t1, t2);

   return true;
}

template < typename T >
inline bool ShaderProgram::SetUniformValue( const GLint uniform, const T & t1, const T & t2, const T & t3 )
{
   WGL_ASSERT(mShaderProg && ShaderProgram::GetCurrentProgram() == mShaderProg);

   UniformValueSelector< T >::SetValue(uniform, t1, t2, t3);

   return true;
}

template < typename T >
inline bool ShaderProgram::SetUniformValue( const GLint uniform, const T & t1, const T & t2, const T & t3, const T & t4 )
{
   WGL_ASSERT(mShaderProg && ShaderProgram::GetCurrentProgram() == mShaderProg);

   UniformValueSelector< T >::SetValue(uniform, t1, t2, t3, t4);

   return true;
}

template < typename T >
inline bool ShaderProgram::SetUniformValue( const std::string & uniform, const T & t1 )
{
   bool modified = false;

   const GLint uniform_loc = GetUniformLocation(uniform);

   if (INVALID_UNIFORM_LOCATION != uniform_loc)
   {
      modified = SetUniformValue(uniform_loc, t1);
   }

   return modified;
}

template < typename T >
inline bool ShaderProgram::SetUniformValue( const std::string & uniform, const T & t1, const T & t2 )
{
   bool modified = false;

   const GLint uniform_loc = GetUniformLocation(uniform);

   if (INVALID_UNIFORM_LOCATION != uniform_loc)
   {
      modified = SetUniformValue(uniform_loc, t1, t2);
   }

   return modified;
}

template < typename T >
inline bool ShaderProgram::SetUniformValue( const std::string & uniform, const T & t1, const T & t2, const T & t3 )
{
   bool modified = false;

   const GLint uniform_loc = GetUniformLocation(uniform);

   if (INVALID_UNIFORM_LOCATION != uniform_loc)
   {
      modified = SetUniformValue(uniform_loc, t1, t2, t3);
   }

   return modified;
}

template < typename T >
inline bool ShaderProgram::SetUniformValue( const std::string & uniform, const T & t1, const T & t2, const T & t3, const T & t4 )
{
   bool modified = false;

   const GLint uniform_loc = GetUniformLocation(uniform);

   if (INVALID_UNIFORM_LOCATION != uniform_loc)
   {
      modified = SetUniformValue(uniform_loc, t1, t2, t3, t4);
   }

   return modified;
}

#endif // _SHADER_PROGRAM_H_
