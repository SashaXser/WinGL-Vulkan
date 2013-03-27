// local includes
#include "Shaders.h"

// stl includes
#include <fstream>
#include <sstream>
#include <iostream>

#include <stdint.h>

namespace shader
{

GLuint LoadShaderSrc( const GLenum type, const std::string & src )
{
   // create the shader object
   const GLuint sobj = glCreateShader(type);

   // set the compiled shader source
   const char * pSrc = src.c_str();
   glShaderSource(sobj, 1, &pSrc, nullptr);

   // compile the shader source
   glCompileShader(sobj);

   // make sure there are no errors
   GLint compiled = GL_FALSE;
   glGetShaderiv(sobj, GL_COMPILE_STATUS, &compiled);

   if (compiled == GL_FALSE)
   {
      // get the error message...
      std::stringstream err;
      err << "Error compiling:" << std::endl
          << src << std::endl << std::endl
          << "Error:" << std::endl;

      char errmsg[1024] = { 0 };
      glGetShaderInfoLog(sobj, sizeof(errmsg), nullptr, errmsg);

      err << errmsg;

      // print the message out
      std::cout << err.str();

      // release the shader object
      glDeleteShader(sobj);

      // return the null shader
      return 0;
   }

   return sobj;
}

GLuint LoadShaderFile( const GLenum type, const std::string & file )
{
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
      std::string src(static_cast< uint32_t >(size), '\0');
      input.read(&src[0], size);

      // load the shader src
      return LoadShaderSrc(type, src);
   }

   // return the null shader
   return 0;
}

bool LinkShaders( const GLuint prog, const GLuint vert, const GLuint geom, const GLuint frag )
{
   // attach all the shaders
   if (vert) glAttachShader(prog, vert);
   if (geom) glAttachShader(prog, geom);
   if (frag) glAttachShader(prog, frag);

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
