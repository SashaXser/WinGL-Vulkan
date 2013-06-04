#ifndef _SHADERS_H_
#define _SHADERS_H_

// platform includes
#include "Window.h"

// gl includes
#include "GL/glew.h"
#include <GL/GL.h>

// stl includes
#include <string>

namespace shader
{

GLuint LoadShaderSrc( const GLenum type, const std::string & src );

GLuint LoadShaderFile( const GLenum type, const std::string & file );

bool LinkShaders( const GLuint prog, const GLuint vert, const GLuint geom, const GLuint frag );

} // namespace shader

#endif // _SHADERS_H_
