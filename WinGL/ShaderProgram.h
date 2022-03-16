#ifndef _SHADER_PROGRAM_H_
#define _SHADER_PROGRAM_H_

// local includes
#include "Matrix.h"
#include "WglAssert.h"

// platform includes
#include "Window.h"

// gl includes
#include "GL/glew.h"
#include <GL/GL.h>

// std includes
#include <map>
#include <string>
#include <vector>
#include <cstring>
#include <algorithm>
#include <type_traits>

// defines an invalid uniform location
extern const GLint INVALID_UNIFORM_LOCATION;

// defines an invalid program id
extern const GLuint INVALID_PROGRAM;

class ShaderProgram
{
private:
   // private structures
   // determines if a variable has a floating point operator call
   template < typename T >
   struct has_floating_point_pointer_operator
   {
      typedef char yes_type;
      typedef long no_type;

      template < typename U, U > struct validator { };

      template < typename V > static yes_type validate( const validator< float * (V::*) ( ), &V::operator float * > * );
      template < typename V > static yes_type validate( const validator< double * (V::*) ( ), &V::operator double * > * );

      template < typename V > static no_type validate( ... );

      static const bool value = sizeof(validate< T >(nullptr)) == sizeof(yes_type);
   };

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

   // attaches the shader to the program
   bool Attach( const GLenum shader, const GLuint shader_obj );
   bool Attach( const GLenum shader, const std::string & src );
   bool Attach( const GLenum shader, const std::vector< std::string > & src );
   bool AttachFile( const GLenum shader, const std::string & file );
   bool AttachFile( const GLenum shader, const std::vector< std::string > & file );

   // attaches source to a specified name
   // this allows the shader to use #include "some_file.glsl" for reusable code
   // this is not in the core and is only an extension, so this will fail depending on the gl context
   bool AddIncludeSrc( const std::string & src, const std::string & include_name );
   bool AddIncludeFile( const std::string & file, const std::string & include_name = "" );

   // get shader from the program
   std::vector< GLuint > GetAttachedShaders( const GLenum shader ) const;

   // gets the shader source for the specified shader
   std::string GetShaderSource( const GLenum shader ) const;

   // links all the attached shaders together
   bool Link( );

   // enables / disables the use of the shader
   void Enable( );
   void Disable( );

   // indicates if the program is active
   bool IsEnabled( ) const;

   // gets a uniforms location
   GLint GetUniformLocation( const std::string & uniform );

   // gets all active uniforms
   std::vector< std::string > GetActiveUniforms( );

   // sets the transform feedback varyings name to allow data
   // to be written to the transform feeback buffers
   void TransformFeedbackVaryings( const char * const * const pVaryings, const GLsizei count, const GLenum buffer_mode );
   void TransformFeedbackVaryings( const std::vector< const char * > & varyings, const GLenum buffer_mode );
   void TransformFeedbackVaryings( const std::vector< std::string > & varyings, const GLenum buffer_mode );

   // gets a uniforms value
   template < typename T > bool GetUniformValue( const GLint uniform, T & t ) const;
   template < typename T > bool GetUniformValue( const GLint uniform, Matrix< T > & t ) const;
   template < typename T > bool GetUniformValue( const std::string & uniform, T & t );

   // sets a uniforms value
   template < typename T > bool SetUniformValue( const GLint uniform, const T & t1,
                                                 const typename std::enable_if< !has_floating_point_pointer_operator< T >::value >::type * const unused = nullptr );
   template < typename T > bool SetUniformValue( const GLint uniform, const T & t1,
                                                 const typename std::enable_if< has_floating_point_pointer_operator< T >::value >::type * const unused = nullptr );
   template < typename T > bool SetUniformValue( const GLint uniform, const T & t1, const T & t2 );
   template < typename T > bool SetUniformValue( const GLint uniform, const T & t1, const T & t2, const T & t3 );
   template < typename T > bool SetUniformValue( const GLint uniform, const T & t1, const T & t2, const T & t3, const T & t4 );
   template < GLsizei COUNT, typename T > bool SetUniformValue( const GLint uniform, const T & t1 );
   template < GLsizei SIZE, typename T > bool SetUniformValue( const GLint uniform, const T & t1, const GLsizei count );
   
   template < GLsizei COUNT, size_t COL, size_t ROW, typename T >
   bool SetUniformMatrix( const GLint uniform, const T & t1, const GLboolean transpose = false,
                          const typename std::enable_if< has_floating_point_pointer_operator< T >::value >::type * const unused = nullptr );
   template < GLsizei COUNT, size_t COL, size_t ROW, typename T >
   bool SetUniformMatrix( const GLint uniform, const T & t1, const GLboolean transpose = false,
                          const typename std::enable_if< !has_floating_point_pointer_operator< T >::value >::type * const unused = nullptr );
   template < typename T, size_t COL, size_t ROW > bool SetUniformMatrix( const GLint uniform, const T (&t1)[COL][ROW], const GLboolean transpose = false );
   template < typename T, GLsizei COUNT, size_t COL, size_t ROW > bool SetUniformMatrix( const GLint uniform, const T (&t1)[COUNT][COL][ROW], const GLboolean transpose = false );

   template < typename T > bool SetUniformValue( const std::string & uniform, const T & t1 );
   template < typename T > bool SetUniformValue( const std::string & uniform, const T & t1, const T & t2 );
   template < typename T > bool SetUniformValue( const std::string & uniform, const T & t1, const T & t2, const T & t3 );
   template < typename T > bool SetUniformValue( const std::string & uniform, const T & t1, const T & t2, const T & t3, const T & t4 );
   template < GLsizei COUNT, typename T > bool SetUniformValue( const std::string & uniform, const T & t1 );
   template < GLsizei SIZE, typename T > bool SetUniformValue( const std::string & uniform, const T & t1, const GLsizei count );

   template < GLsizei COUNT, size_t COL, size_t ROW, typename T > bool SetUniformMatrix( const std::string & uniform, const T & t1, const GLboolean transpose = false );
   template < typename T, size_t COL, size_t ROW > bool SetUniformMatrix( const std::string & uniform, const T (&t1)[COL][ROW], const GLboolean transpose = false );
   template < typename T, GLsizei COUNT, size_t COL, size_t ROW > bool SetUniformMatrix( const std::string & uniform, const T (&t1)[COUNT][COL][ROW], const GLboolean transpose = false );

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

   static void SetValue( const GLint loc, const type * const pValues, const array_selector< true, sizeof(type) * 1 > ) { glUniform1f(loc, pValues[0]); }
   static void SetValue( const GLint loc, const type * const pValues, const array_selector< true, sizeof(type) * 2 > ) { glUniform2f(loc, pValues[0], pValues[1]); }
   static void SetValue( const GLint loc, const type * const pValues, const array_selector< true, sizeof(type) * 3 > ) { glUniform3f(loc, pValues[0], pValues[1], pValues[2]); }
   static void SetValue( const GLint loc, const type * const pValues, const array_selector< true, sizeof(type) * 4 > ) { glUniform4f(loc, pValues[0], pValues[1], pValues[2], pValues[3]); }

   static void SetValue( const GLint loc, const type * const pValues, const GLsizei count, const array_selector< true, sizeof(type) * 1 > ) { glUniform1fv(loc, count, pValues); }
   static void SetValue( const GLint loc, const type * const pValues, const GLsizei count, const array_selector< true, sizeof(type) * 2 > ) { glUniform2fv(loc, count, pValues); }
   static void SetValue( const GLint loc, const type * const pValues, const GLsizei count, const array_selector< true, sizeof(type) * 3 > ) { glUniform3fv(loc, count, pValues); }
   static void SetValue( const GLint loc, const type * const pValues, const GLsizei count, const array_selector< true, sizeof(type) * 4 > ) { glUniform4fv(loc, count, pValues); }

   template < size_t COL, size_t ROW > struct array_dims { };

   static void SetMatrix( const GLint loc, const type * const pValues, const GLsizei count, const GLboolean transpose, const array_dims< 2, 2 > ) { glUniformMatrix2fv(loc, count, transpose, pValues); }
   static void SetMatrix( const GLint loc, const type * const pValues, const GLsizei count, const GLboolean transpose, const array_dims< 3, 3 > ) { glUniformMatrix3fv(loc, count, transpose, pValues); }
   static void SetMatrix( const GLint loc, const type * const pValues, const GLsizei count, const GLboolean transpose, const array_dims< 4, 4 > ) { glUniformMatrix4fv(loc, count, transpose, pValues); }
   static void SetMatrix( const GLint loc, const type * const pValues, const GLsizei count, const GLboolean transpose, const array_dims< 2, 3 > ) { glUniformMatrix2x3fv(loc, count, transpose, pValues); }
   static void SetMatrix( const GLint loc, const type * const pValues, const GLsizei count, const GLboolean transpose, const array_dims< 3, 2 > ) { glUniformMatrix3x2fv(loc, count, transpose, pValues); }
   static void SetMatrix( const GLint loc, const type * const pValues, const GLsizei count, const GLboolean transpose, const array_dims< 2, 4 > ) { glUniformMatrix2x4fv(loc, count, transpose, pValues); }
   static void SetMatrix( const GLint loc, const type * const pValues, const GLsizei count, const GLboolean transpose, const array_dims< 4, 2 > ) { glUniformMatrix4x2fv(loc, count, transpose, pValues); }
   static void SetMatrix( const GLint loc, const type * const pValues, const GLsizei count, const GLboolean transpose, const array_dims< 3, 4 > ) { glUniformMatrix3x4fv(loc, count, transpose, pValues); }
   static void SetMatrix( const GLint loc, const type * const pValues, const GLsizei count, const GLboolean transpose, const array_dims< 4, 3 > ) { glUniformMatrix4x3fv(loc, count, transpose, pValues); }
};

template < > struct ShaderProgram::UniformValueSelector< GLdouble >
{
   typedef GLdouble type;

   static const type INVALID_VALUE;
   static bool IsValid( const type & value ) { return INVALID_VALUE != value; }
   static bool IsValid( const type * pValue ) { return INVALID_VALUE != *pValue; }

   static void GetValue( const GLuint prog, const GLint loc, type & value ) { glGetUniformdv(prog, loc, &value); }
   static void GetValue( const GLuint prog, const GLint loc, type * pValue ) { glGetUniformdv(prog, loc, pValue); }

   template < bool IS_ARRAY, size_t SIZE > struct array_selector { };

   static void SetValue( const GLint loc, const type & v1, const array_selector< false, 4 > ) { glUniform1d(loc, v1); }
   static void SetValue( const GLint loc, const type & v1, const type & v2 ) { glUniform2d(loc, v1, v2); }
   static void SetValue( const GLint loc, const type & v1, const type & v2, const type & v3 ) { glUniform3d(loc, v1, v2, v3); }
   static void SetValue( const GLint loc, const type & v1, const type & v2, const type & v3, const type & v4 ) { glUniform4d(loc, v1, v2, v3, v4); }

   static void SetValue( const GLint loc, const type * const pValues, const array_selector< true, sizeof(type) * 1 > ) { glUniform1d(loc, pValues[0]); }
   static void SetValue( const GLint loc, const type * const pValues, const array_selector< true, sizeof(type) * 2 > ) { glUniform2d(loc, pValues[0], pValues[1]); }
   static void SetValue( const GLint loc, const type * const pValues, const array_selector< true, sizeof(type) * 3 > ) { glUniform3d(loc, pValues[0], pValues[1], pValues[2]); }
   static void SetValue( const GLint loc, const type * const pValues, const array_selector< true, sizeof(type) * 4 > ) { glUniform4d(loc, pValues[0], pValues[1], pValues[2], pValues[3]); }

   static void SetValue( const GLint loc, const type * const pValues, const GLsizei count, const array_selector< true, sizeof(type) * 1 > ) { glUniform1dv(loc, count, pValues); }
   static void SetValue( const GLint loc, const type * const pValues, const GLsizei count, const array_selector< true, sizeof(type) * 2 > ) { glUniform2dv(loc, count, pValues); }
   static void SetValue( const GLint loc, const type * const pValues, const GLsizei count, const array_selector< true, sizeof(type) * 3 > ) { glUniform3dv(loc, count, pValues); }
   static void SetValue( const GLint loc, const type * const pValues, const GLsizei count, const array_selector< true, sizeof(type) * 4 > ) { glUniform4dv(loc, count, pValues); }

   template < size_t COL, size_t ROW > struct array_dims { };

   static void SetMatrix( const GLint loc, const type * const pValues, const GLsizei count, const GLboolean transpose, const array_dims< 2, 2 > ) { glUniformMatrix2dv(loc, count, transpose, pValues); }
   static void SetMatrix( const GLint loc, const type * const pValues, const GLsizei count, const GLboolean transpose, const array_dims< 3, 3 > ) { glUniformMatrix3dv(loc, count, transpose, pValues); }
   static void SetMatrix( const GLint loc, const type * const pValues, const GLsizei count, const GLboolean transpose, const array_dims< 4, 4 > ) { glUniformMatrix4dv(loc, count, transpose, pValues); }
   static void SetMatrix( const GLint loc, const type * const pValues, const GLsizei count, const GLboolean transpose, const array_dims< 2, 3 > ) { glUniformMatrix2x3dv(loc, count, transpose, pValues); }
   static void SetMatrix( const GLint loc, const type * const pValues, const GLsizei count, const GLboolean transpose, const array_dims< 3, 2 > ) { glUniformMatrix3x2dv(loc, count, transpose, pValues); }
   static void SetMatrix( const GLint loc, const type * const pValues, const GLsizei count, const GLboolean transpose, const array_dims< 2, 4 > ) { glUniformMatrix2x4dv(loc, count, transpose, pValues); }
   static void SetMatrix( const GLint loc, const type * const pValues, const GLsizei count, const GLboolean transpose, const array_dims< 4, 2 > ) { glUniformMatrix4x2dv(loc, count, transpose, pValues); }
   static void SetMatrix( const GLint loc, const type * const pValues, const GLsizei count, const GLboolean transpose, const array_dims< 3, 4 > ) { glUniformMatrix3x4dv(loc, count, transpose, pValues); }
   static void SetMatrix( const GLint loc, const type * const pValues, const GLsizei count, const GLboolean transpose, const array_dims< 4, 3 > ) { glUniformMatrix4x3dv(loc, count, transpose, pValues); }

};

template < > struct ShaderProgram::UniformValueSelector< GLint >
{
   typedef GLint type;

   static const type INVALID_VALUE;
   static bool IsValid( const type & value ) { return INVALID_VALUE != value; }
   static bool IsValid( const type * pValue ) { return INVALID_VALUE != *pValue; }

   static void GetValue( const GLuint prog, const GLint loc, type & value ) { glGetUniformiv(prog, loc, &value); }
   static void GetValue( const GLuint prog, const GLint loc, type * pValue ) { glGetUniformiv(prog, loc, pValue); }

   template < bool IS_ARRAY, size_t SIZE > struct array_selector { };

   static void SetValue( const GLint loc, const type & v1, const array_selector< false, 4 > ) { glUniform1i(loc, v1); }
   static void SetValue( const GLint loc, const type & v1, const type & v2 ) { glUniform2i(loc, v1, v2); }
   static void SetValue( const GLint loc, const type & v1, const type & v2, const type & v3 ) { glUniform3i(loc, v1, v2, v3); }
   static void SetValue( const GLint loc, const type & v1, const type & v2, const type & v3, const type & v4 ) { glUniform4i(loc, v1, v2, v3, v4); }

   static void SetValue( const GLint loc, const type * const pValues, const array_selector< true, sizeof(type) * 1 > ) { glUniform1i(loc, pValues[0]); }
   static void SetValue( const GLint loc, const type * const pValues, const array_selector< true, sizeof(type) * 2 > ) { glUniform2i(loc, pValues[0], pValues[1]); }
   static void SetValue( const GLint loc, const type * const pValues, const array_selector< true, sizeof(type) * 3 > ) { glUniform3i(loc, pValues[0], pValues[1], pValues[2]); }
   static void SetValue( const GLint loc, const type * const pValues, const array_selector< true, sizeof(type) * 4 > ) { glUniform4i(loc, pValues[0], pValues[1], pValues[2], pValues[3]); }

   static void SetValue( const GLint loc, const type * const pValues, const GLsizei count, const array_selector< true, sizeof(type) * 1 > ) { glUniform1iv(loc, count, pValues); }
   static void SetValue( const GLint loc, const type * const pValues, const GLsizei count, const array_selector< true, sizeof(type) * 2 > ) { glUniform2iv(loc, count, pValues); }
   static void SetValue( const GLint loc, const type * const pValues, const GLsizei count, const array_selector< true, sizeof(type) * 3 > ) { glUniform3iv(loc, count, pValues); }
   static void SetValue( const GLint loc, const type * const pValues, const GLsizei count, const array_selector< true, sizeof(type) * 4 > ) { glUniform4iv(loc, count, pValues); }
};

template < > struct ShaderProgram::UniformValueSelector< GLuint >
{
   typedef GLuint type;

   static const type INVALID_VALUE;
   static bool IsValid( const type & value ) { return INVALID_VALUE != value; }
   static bool IsValid( const type * pValue ) { return INVALID_VALUE != *pValue; }

   static void GetValue( const GLuint prog, const GLint loc, type & value ) { glGetUniformuiv(prog, loc, &value); }
   static void GetValue( const GLuint prog, const GLint loc, type * pValue ) { glGetUniformuiv(prog, loc, pValue); }

   template < bool IS_ARRAY, size_t SIZE > struct array_selector { };

   static void SetValue( const GLint loc, const type & v1, const array_selector< false, 4 > ) { glUniform1ui(loc, v1); }
   static void SetValue( const GLint loc, const type & v1, const type & v2 ) { glUniform2ui(loc, v1, v2); }
   static void SetValue( const GLint loc, const type & v1, const type & v2, const type & v3 ) { glUniform3ui(loc, v1, v2, v3); }
   static void SetValue( const GLint loc, const type & v1, const type & v2, const type & v3, const type & v4 ) { glUniform4ui(loc, v1, v2, v3, v4); }

   static void SetValue( const GLint loc, const type * const pValues, const array_selector< true, sizeof(type) * 1 > ) { glUniform1ui(loc, pValues[0]); }
   static void SetValue( const GLint loc, const type * const pValues, const array_selector< true, sizeof(type) * 2 > ) { glUniform2ui(loc, pValues[0], pValues[1]); }
   static void SetValue( const GLint loc, const type * const pValues, const array_selector< true, sizeof(type) * 3 > ) { glUniform3ui(loc, pValues[0], pValues[1], pValues[2]); }
   static void SetValue( const GLint loc, const type * const pValues, const array_selector< true, sizeof(type) * 4 > ) { glUniform4ui(loc, pValues[0], pValues[1], pValues[2], pValues[3]); }

   static void SetValue( const GLint loc, const type * const pValues, const GLsizei count, const array_selector< true, sizeof(type) * 1 > ) { glUniform1uiv(loc, count, pValues); }
   static void SetValue( const GLint loc, const type * const pValues, const GLsizei count, const array_selector< true, sizeof(type) * 2 > ) { glUniform2uiv(loc, count, pValues); }
   static void SetValue( const GLint loc, const type * const pValues, const GLsizei count, const array_selector< true, sizeof(type) * 3 > ) { glUniform3uiv(loc, count, pValues); }
   static void SetValue( const GLint loc, const type * const pValues, const GLsizei count, const array_selector< true, sizeof(type) * 4 > ) { glUniform4uiv(loc, count, pValues); }
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
inline bool ShaderProgram::GetUniformValue( const GLint uniform, Matrix< T > & t ) const
{
   WGL_ASSERT(mShaderProg);

   typedef Matrix< T >::type Type;

   bool obtained = false;

   Matrix< T > value = { UniformValueSelector< Type >::INVALID_VALUE };

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

// defines a macro to do some validatation on the sets
#define __VALIDATE_SHADER_UNIFORM( uniform ) \
   WGL_ASSERT(mShaderProg && ShaderProgram::GetCurrentProgram() == mShaderProg); \
   WGL_ASSERT_INIT(const auto active_uniforms = GetActiveUniforms(); \
                   const auto active_uniform = std::find_if(active_uniforms.cbegin(), active_uniforms.cend(), \
                   [ this, &uniform ] ( const std::string & active_uniform ) \
                   { \
                      return GetUniformLocation(active_uniform) == uniform; \
                   });, \
                   active_uniform != active_uniforms.cend());

template < typename T >
inline bool ShaderProgram::SetUniformValue( const GLint uniform, const T & t1,
                                            const typename std::enable_if< !has_floating_point_pointer_operator< T >::value >::type * const )
{
   __VALIDATE_SHADER_UNIFORM(uniform);

   typedef std::remove_const< std::remove_extent< T >::type >::type Type;
   typedef UniformValueSelector< Type >::template array_selector< std::is_array< T >::value, sizeof(t1) > array_selector;

   UniformValueSelector< Type >::SetValue(uniform, t1, array_selector());

   return true;
}

template < typename T >
inline bool ShaderProgram::SetUniformValue( const GLint uniform, const T & t1,
                                            const typename std::enable_if< has_floating_point_pointer_operator< T >::value >::type * const )
{
   __VALIDATE_SHADER_UNIFORM(uniform);

   typedef UniformValueSelector< T::type >::template array_selector< true, sizeof(t1) > array_selector;

   UniformValueSelector< T::type >::SetValue(uniform, t1, array_selector());

   return true;
}

template < typename T >
inline bool ShaderProgram::SetUniformValue( const GLint uniform, const T & t1, const T & t2 )
{
   __VALIDATE_SHADER_UNIFORM(uniform);

   UniformValueSelector< T >::SetValue(uniform, t1, t2);

   return true;
}

template < typename T >
inline bool ShaderProgram::SetUniformValue( const GLint uniform, const T & t1, const T & t2, const T & t3 )
{
   __VALIDATE_SHADER_UNIFORM(uniform);

   UniformValueSelector< T >::SetValue(uniform, t1, t2, t3);

   return true;
}

template < typename T >
inline bool ShaderProgram::SetUniformValue( const GLint uniform, const T & t1, const T & t2, const T & t3, const T & t4 )
{
   __VALIDATE_SHADER_UNIFORM(uniform);

   UniformValueSelector< T >::SetValue(uniform, t1, t2, t3, t4);

   return true;
}

template < GLsizei COUNT, typename T >
inline bool ShaderProgram::SetUniformValue( const GLint uniform, const T & t1 )
{
   __VALIDATE_SHADER_UNIFORM(uniform);

   typedef std::remove_const< std::remove_extent< T >::type >::type Type;
   typedef UniformValueSelector< Type >::template array_selector< std::is_array< T >::value, sizeof(T) / COUNT > array_selector;

   UniformValueSelector< Type >::SetValue(uniform, t1, COUNT, array_selector());

   return true;
}

template < GLsizei SIZE, typename T >
inline bool ShaderProgram::SetUniformValue( const GLint uniform, const T & t1, const GLsizei count )
{
   __VALIDATE_SHADER_UNIFORM(uniform);

   typedef std::remove_const< std::remove_pointer< T >::type >::type Type;
   typedef UniformValueSelector< Type >::template array_selector< std::is_pointer< T >::value, SIZE * sizeof(Type) > array_selector;

   UniformValueSelector< Type >::SetValue(uniform, t1, count, array_selector());

   return true;
}

template < GLsizei COUNT, size_t COL, size_t ROW, typename T >
inline bool ShaderProgram::SetUniformMatrix( const GLint uniform, const T & t1, const GLboolean transpose,
                                             const typename std::enable_if< has_floating_point_pointer_operator< T >::value >::type * const )
{
   __VALIDATE_SHADER_UNIFORM(uniform);

   typedef UniformValueSelector< T::type >::template array_dims< COL, ROW > array_dims;

   UniformValueSelector< T::type >::SetMatrix(uniform, t1, COUNT, transpose, array_dims());

   return true;
}

template < GLsizei COUNT, size_t COL, size_t ROW, typename T >
inline bool ShaderProgram::SetUniformMatrix( const GLint uniform, const T & t1, const GLboolean transpose,
                                             const typename std::enable_if< !has_floating_point_pointer_operator< T >::value >::type * const )
{
   __VALIDATE_SHADER_UNIFORM(uniform);

   typedef std::remove_const< std::remove_pointer< std::remove_extent< T >::type >::type >::type Type;
   typedef UniformValueSelector< Type >::template array_dims< COL, ROW > array_dims;

   UniformValueSelector< Type >::SetMatrix(uniform, t1, COUNT, transpose, array_dims());

   return true;
}

template < typename T, size_t COL, size_t ROW >
inline bool ShaderProgram::SetUniformMatrix( const GLint uniform, const T (&t1)[COL][ROW], const GLboolean transpose )
{
   __VALIDATE_SHADER_UNIFORM(uniform);

   typedef UniformValueSelector< T >::template array_dims< COL, ROW > array_dims;

   UniformValueSelector< T >::SetMatrix(uniform, *t1, 1, transpose, array_dims());

   return true;
}

template < typename T, GLsizei COUNT, size_t COL, size_t ROW >
inline bool ShaderProgram::SetUniformMatrix( const GLint uniform, const T (&t1)[COUNT][COL][ROW], const GLboolean transpose )
{
   __VALIDATE_SHADER_UNIFORM(uniform);

   typedef UniformValueSelector< T >::template array_dims< COL, ROW > array_dims;

   UniformValueSelector< T >::SetMatrix(uniform, **t1, COUNT, transpose, array_dims());

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

template < GLsizei COUNT, typename T >
inline bool ShaderProgram::SetUniformValue( const std::string & uniform, const T & t1 )
{
   bool modified = false;

   const GLint uniform_loc = GetUniformLocation(uniform);

   if (INVALID_UNIFORM_LOCATION != uniform_loc)
   {
      modified = SetUniformValue< COUNT >(uniform_loc, t1);
   }

   return modified;
}

template < GLsizei SIZE, typename T >
inline bool ShaderProgram::SetUniformValue( const std::string & uniform, const T & t1, const GLsizei count )
{
   bool modified = false;

   const GLint uniform_loc = GetUniformLocation(uniform);

   if (INVALID_UNIFORM_LOCATION != uniform_loc)
   {
      modified = SetUniformValue< SIZE >(uniform_loc, t1, count);
   }

   return modified;
}

template < GLsizei COUNT, size_t COL, size_t ROW, typename T >
inline bool ShaderProgram::SetUniformMatrix( const std::string & uniform, const T & t1, const GLboolean transpose )
{
   bool modified = false;

   const GLint uniform_loc = GetUniformLocation(uniform);

   if (INVALID_UNIFORM_LOCATION != uniform_loc)
   {
      modified = SetUniformMatrix< COUNT, COL, ROW >(uniform_loc, t1, transpose);
   }

   return modified;
}

template < typename T, size_t COL, size_t ROW >
inline bool ShaderProgram::SetUniformMatrix( const std::string & uniform, const T (&t1)[COL][ROW], const GLboolean transpose )
{
   bool modified = false;

   const GLint uniform_loc = GetUniformLocation(uniform);

   if (INVALID_UNIFORM_LOCATION != uniform_loc)
   {
      modified = SetUniformMatrix(uniform_loc, t1, transpose);
   }

   return modified;
}

template < typename T, GLsizei COUNT, size_t COL, size_t ROW >
inline bool ShaderProgram::SetUniformMatrix( const std::string & uniform, const T (&t1)[COUNT][COL][ROW], const GLboolean transpose )
{
   bool modified = false;

   const GLint uniform_loc = GetUniformLocation(uniform);

   if (INVALID_UNIFORM_LOCATION != uniform_loc)
   {
      modified = SetUniformMatrix(uniform_loc, t1, transpose);
   }

   return modified;
}

#endif // _SHADER_PROGRAM_H_
