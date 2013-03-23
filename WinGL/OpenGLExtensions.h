#ifndef _OPENGL_EXTENSIONS_H_
#define _OPENGL_EXTENSIONS_H_

// platform includes
#include <windows.h>

namespace gl
{

// initializes gl extensions
// the function passed in is a function pointer assigned the name wglGetProcAddress
// which accepts a const char *...  the returning value is another function pointer
// that returns int and take no arguments...
void InitializeOpenGLExtensions( );

// determines if an extension is supported
bool IsExtensionSupported( const char * const pExtension );

}; // namespace gl

#endif // _OPENGL_EXTENSIONS_H_
