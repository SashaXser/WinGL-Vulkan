#ifndef _SHADERS_H_
#define _SHADERS_H_

// platform includes
#include "Window.h"

// gl includes
#include "GL/glew.h"
#include <GL/GL.h>

// stl includes
#include <string>
#include <vector>

namespace shader
{

GLuint LoadShaderSrc( const GLenum type, const std::string & src );
GLuint LoadShaderSrc( const GLenum type, const std::vector< const std::string > & src );

GLuint LoadShaderFile( const GLenum type, const std::string & file );
GLuint LoadShaderFile( const GLenum type, const std::vector< const std::string > & file );

bool LinkShaders( const GLuint prog,
                  const GLuint vert, const GLuint geom, const GLuint frag,
                  const GLuint tess_ctrl = 0, const GLuint tess_eval = 0 );

bool LinkShader( const GLuint prog );

} // namespace shader

#endif // _SHADERS_H_
