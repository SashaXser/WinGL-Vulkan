// local includes
#include "ShaderProgram.h"
#include "Shaders.h"

// std includes
#include <algorithm>

// defines an invalid uniform location
const GLint INVALID_UNIFORM_LOCATION = -1;

// defines an invalid program id
const GLuint INVALID_PROGRAM = 0;

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
      // must happen within a valid gl context
      WGL_ASSERT(wglGetCurrentContext());

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

bool ShaderProgram::Attach( const GLenum shader, const GLuint shader_obj )
{
   bool attached = false;

   if ((mShaderProg || (mShaderProg = glCreateProgram())) && shader_obj)
   {
      // attach the shader to the program...
      glAttachShader(mShaderProg, shader_obj);

      attached = true;
   }

   return attached;
}

bool ShaderProgram::Attach( const GLenum shader, const std::string & src )
{
   return Attach(shader, std::vector< std::string > { src });
}

bool ShaderProgram::Attach( const GLenum shader, const std::vector< std::string > & src )
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

         attached = true;
      }
   }

   return attached;
}

bool ShaderProgram::AttachFile( const GLenum shader, const std::string & file )
{
   return AttachFile(shader, std::vector< std::string > { file });
}

bool ShaderProgram::AttachFile( const GLenum shader, const std::vector< std::string > & file )
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

         attached = true;
      }
   }

   return attached;
}

bool ShaderProgram::AddIncludeSrc( const std::string & src, const std::string & include_name )
{
   // untested code (as the card does not support or driver not updated)

   // this extension needs to be present to work
   WGL_ASSERT_REPORT(glNamedStringARB, "ShaderProgram::AddIncludeSrc : glNamedStringARB Is Not Supported");

   bool include_added = false;

   if (glNamedStringARB && !include_name.empty())
   {
      // must have '/' as first character
      const std::string name = include_name[0] == '/' ? include_name : "/" + include_name;

      // bind the source to the specified name
      glNamedStringARB(GL_SHADER_INCLUDE_ARB,
                       static_cast< GLsizei >(name.size()), name.c_str(),
                       static_cast< GLsizei >(src.size()), src.c_str());

      // source added for includes
      include_added = true;
   }

   return include_added;
}

bool ShaderProgram::AddIncludeFile( const std::string & file, const std::string & include_name )
{
   // untested code (as the card does not support or driver not updated)

   // must have '/' as first character
   const std::string name = include_name.empty() ? file : include_name;

   return AddIncludeSrc(shader::Read(file), name);
}

std::vector< GLuint > ShaderProgram::GetAttachedShaders( const GLenum shader ) const
{
   std::vector< GLuint > shaders;

   if (mShaderProg)
   {
      // obtain the maximum number of attached shaders
      const GLint num_attached_shaders = [ this ] ( ) -> GLint
      {
         GLint num_attached_shaders = 0;
         glGetProgramiv(mShaderProg, GL_ATTACHED_SHADERS, &num_attached_shaders);

         return num_attached_shaders;
      }();

      if (num_attached_shaders)
      {
         // obtain all the shader names attached to this program
         shaders.resize(num_attached_shaders);
         glGetAttachedShaders(mShaderProg, num_attached_shaders, nullptr, &shaders[0]);

         // validate for the existance of the shader type requested
         const auto end = std::remove_if(shaders.begin(), shaders.end(),
         [ &shader ] ( const GLint shader_obj ) -> bool
         {
            // obtain the shader type
            GLint shader_type = 0;
            glGetShaderiv(shader_obj, GL_SHADER_TYPE, &shader_type);
            
            return shader != shader_type;
         });

         // resize to only include those types that match shader
         shaders.erase(end, shaders.end());
      }
   }

   return shaders;
}

std::string ShaderProgram::GetShaderSource( const GLenum shader ) const
{
   std::string shader_source;

   // get the size of the shader source
   const GLsizei source_length =
   [ shader ] ( ) -> GLsizei
   {
      GLsizei source_length = 0;
      glGetShaderiv(shader, GL_SHADER_SOURCE_LENGTH, &source_length);

      return source_length;
   }();

   if (source_length)
   {
      // resize the shader source
      shader_source.resize(source_length - 1);

      // fill in the shader source
      glGetShaderSource(shader, source_length, nullptr, &shader_source[0]);
   }

   return shader_source;
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

std::vector< std::string > ShaderProgram::GetActiveUniforms()
{
   WGL_ASSERT(mShaderProg);

   // this process could be more efficient in storing the values
   // but that can be done at a later time...
   std::vector< std::string > active_uniforms;

   const GLint max_uniforms = [ this ] ( )
   {
      GLint max_uniforms = 0;
      glGetProgramiv(mShaderProg, GL_ACTIVE_UNIFORMS, &max_uniforms);
   
      return max_uniforms;
   }();
   
   if (max_uniforms > 0)
   {
      const GLint max_uniforms_size = [ this ] ( )
      {
         GLint max_uniforms_size = 0;
         glGetProgramiv(mShaderProg, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_uniforms_size);
      
         return max_uniforms_size;
      }();
      
      std::vector< char > uniform_name(max_uniforms_size, '\0');
   
      for (GLint i = 0; i < max_uniforms; ++i)
      {
         GLint uniform_size = 0; GLenum uniform_type = 0;
         glGetActiveUniform(mShaderProg, i, max_uniforms_size, nullptr, &uniform_size, &uniform_type, &uniform_name.front());

         active_uniforms.push_back(&uniform_name.front());
      }
   }

   return active_uniforms;
}

void ShaderProgram::Enable( )
{
   if (mShaderProg) glUseProgram(mShaderProg);
}

void ShaderProgram::Disable( )
{
   glUseProgram(0);
}

bool ShaderProgram::IsEnabled( ) const
{
   return ShaderProgram::GetCurrentProgram() == mShaderProg && INVALID_PROGRAM != mShaderProg;
}

