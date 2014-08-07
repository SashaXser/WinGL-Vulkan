// local includes
#include "ShaderProgram.h"
#include "Shaders.h"

// std includes
#include <algorithm>

// defines an invalid uniform location
const GLint INVALID_UNIFORM_LOCATION = -1;

// defines an invalid value when obtaining a uniform value
const GLint ShaderProgram::UniformValueSelector< GLint >::INVALID_VALUE = 134679852;
const GLuint ShaderProgram::UniformValueSelector< GLuint >::INVALID_VALUE = 134679852;
const GLfloat ShaderProgram::UniformValueSelector< GLfloat >::INVALID_VALUE = 123456.648f;
const GLdouble ShaderProgram::UniformValueSelector< GLdouble >::INVALID_VALUE = 123456.648;

GLuint ShaderProgram::GetCurrentProgram( )
{
   GLint program = 0;
   glGetIntegerv(GL_CURRENT_PROGRAM, &program);

   return static_cast< GLuint >(program);
}

ShaderProgram::ShaderProgram( ) :
mShaderProg    ( 0 )
{
}

ShaderProgram::~ShaderProgram( )
{
   if (mShaderProg)
   {
      glDeleteProgram(mShaderProg);
   }
}

ShaderProgram::ShaderProgram( ShaderProgram && program ) :
mShaderProg( 0 )
{
   std::swap(mShaderProg, program.mShaderProg);
   std::swap(mUniformLocCtr, program.mUniformLocCtr);
}

ShaderProgram & ShaderProgram::operator = ( ShaderProgram && program )
{
   if (this != &program)
   {
      std::swap(mShaderProg, program.mShaderProg);
      std::swap(mUniformLocCtr, program.mUniformLocCtr);
   }

   return *this;
}

bool ShaderProgram::Attach( const GLenum shader, const std::string & src )
{
   bool attached = false;

   if (mShaderProg || (mShaderProg = glCreateProgram()))
   {
      if (const GLuint shader_obj = shader::LoadShaderSrc(shader, src))
      {
         // attach the shader to the program...
         glAttachShader(mShaderProg, shader_obj);
         // release the reference from the shader...
         glDeleteShader(shader_obj);
      }

      attached = true;
   }

   return attached;
}

bool ShaderProgram::AttachFile( const GLenum shader, const std::string & file )
{
   bool attached = false;

   if (mShaderProg || (mShaderProg = glCreateProgram()))
   {
      if (const GLuint shader_obj = shader::LoadShaderFile(shader, file))
      {
         // attach the shader to the program...
         glAttachShader(mShaderProg, shader_obj);
         // release the reference from the shader...
         glDeleteShader(shader_obj);
      }

      attached = true;
   }

   return attached;
}

bool ShaderProgram::Link( )
{
   WGL_ASSERT(mShaderProg);

   return mShaderProg && shader::LinkShader(mShaderProg);
}

GLint ShaderProgram::GetUniformLocation( const std::string & uniform )
{
   WGL_ASSERT(mShaderProg);

   GLint uniform_loc = INVALID_UNIFORM_LOCATION;

   if (mShaderProg)
   {
      const auto cached_uniform = mUniformLocCtr.find(uniform);

      if (cached_uniform != mUniformLocCtr.end())
      {
         uniform_loc = cached_uniform->second;
      }

      if (INVALID_UNIFORM_LOCATION == uniform_loc)
      {
         if (INVALID_UNIFORM_LOCATION != (uniform_loc = glGetUniformLocation(mShaderProg, uniform.c_str())))
         {
            mUniformLocCtr.insert(UniformLocationCtr::value_type(uniform, uniform_loc));
         }
      }
   }

   return uniform_loc;
}

void ShaderProgram::Enable( )
{
   if (mShaderProg) glUseProgram(mShaderProg);
}

void ShaderProgram::Disable( )
{
   glUseProgram(0);
}

