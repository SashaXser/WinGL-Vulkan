#ifndef _OPENGL_EXTENSIONS_H_
#define _OPENGL_EXTENSIONS_H_

// platform includes
#include <windows.h>

namespace gl
{

// if wnd is not null, associate a gl context to it;
// owtherwise, a temp window will be created with a gl context
HWND CreateOpenGLWindow( const HWND wnd, HGLRC & context );

// determines if an extension is supported
bool IsExtensionSupported( const char * const pExtension );

}; // namespace gl

#endif // _OPENGL_EXTENSIONS_H_
