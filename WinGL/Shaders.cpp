// local includes
#include "Shaders.h"

// std includes
#include <memory>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <iostream>
#include <algorithm>

// platform includes
#include <windows.h>

namespace shader
{

std::string Read( const std::string & file )
{
   // this function really needs to be in an IO namespace (move at a later time)

   // empty string if unable to open
   std::string src;

   // open the specified file
   std::ifstream input(file);
   
   if (input.is_open())
   {
      // determine the size of the input file
      const std::streamoff beg = input.tellg();
      input.seekg(0, std::ios::end);
      const std::streamoff size = input.tellg() - beg;
      input.seekg(0);
   
      // read the file begin to end
      src.resize(static_cast< uint32_t >(size), '\0');
      input.read(&src[0], size);
   }

   return src;
}

GLuint LoadShaderSrc( const GLenum type, const std::string & src )
{
   return LoadShaderSrc(type, std::vector< const std::string > { src });
}

GLuint LoadShaderSrc( const GLenum type, const std::vector< const std::string > & sources )
{
   // defines the shader object
   GLuint sobj = 0;

   // make sure there is something to work with
   if (!sources.empty())
   {
      // create the shader object
      sobj = glCreateShader(type);

      // combine all the source into an array of pointers
      const std::unique_ptr< const char *[] > source_pointers(new const char * [sources.size()]);
      for (auto source_beg = sources.cbegin(); source_beg != sources.cend(); ++source_beg)
      {
         source_pointers.get()[std::distance(sources.cbegin(), source_beg)] = source_beg->c_str();
      }

      // set the compiled shader source
      glShaderSource(sobj, static_cast< GLsizei >(sources.size()), source_pointers.get(), nullptr);

      // compile the shader source
      glCompileShader(sobj);

      // make sure there are no errors
      GLint compiled = GL_FALSE;
      glGetShaderiv(sobj, GL_COMPILE_STATUS, &compiled);

      if (compiled == GL_FALSE)
      {
         // get the error message...
         std::stringstream err;
         err << "Error compiling:" << std::endl;

         for (auto source_beg = sources.cbegin(); source_beg != sources.cend(); ++source_beg)
         {
            err << *source_beg << std::endl << std::endl;
         }
         
         err << "Error:" << std::endl;

         char errmsg[1024] = { 0 };
         glGetShaderInfoLog(sobj, sizeof(errmsg), nullptr, errmsg);

         err << errmsg;

         // print the message out
         std::cout << err.str();

         // release the shader object
         glDeleteShader(sobj);

         // return the null shader
         sobj = 0;
      }
   }

   return sobj;
}

GLuint LoadShaderFile( const GLenum type, const std::string & file )
{
   return LoadShaderFile(type, std::vector< const std::string > { file });
}

GLuint LoadShaderFile( const GLenum type, const std::vector< const std::string > & file )
{
   // collection of all the source files
   std::vector< const std::string > sources;

   std::for_each(file.cbegin(), file.cend(),
   [ &sources ] ( const std::string & file )
   {
      // read and add to the vector of sources
      sources.push_back(Read(file));
   });

   // load the shader src
   return LoadShaderSrc(type, sources);
}

bool LinkShaders( const GLuint prog,
                  const GLuint vert, const GLuint geom, const GLuint frag,
                  const GLuint tess_ctrl, const GLuint tess_eval )
{
   // attach all the shaders
   if (vert) glAttachShader(prog, vert);
   if (tess_ctrl) glAttachShader(prog, tess_ctrl);
   if (tess_eval) glAttachShader(prog, tess_eval);
   if (geom) glAttachShader(prog, geom);
   if (frag) glAttachShader(prog, frag);

   // link the shaders to the program
   return LinkShader(prog);
}

bool LinkShader( const GLuint prog )
{
   // link the shaders to the program
   glLinkProgram(prog);

   // make sure it is linked
   GLint linked = GL_FALSE;
   glGetProgramiv(prog, GL_LINK_STATUS, &linked);

   if (linked == GL_FALSE)
   {
      // get the error message...
      std::stringstream err;
      err << "Error linking:" << std::endl;

      char errmsg[1024] = { 0 };
      glGetProgramInfoLog(prog, sizeof(errmsg), nullptr, errmsg);

      err << errmsg;

      // print the message out
      std::cout << err.str();

      return false;
   }

   return true;
}

} // namespace shader
