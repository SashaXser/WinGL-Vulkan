#ifndef _OPENGL_EXTENSIONS_H_
#define _OPENGL_EXTENSIONS_H_

// platform includes
#include <windows.h>

// std includes
#include <cstdint>

namespace gl
{

// if wnd is not null, associate a gl context to it;
// owtherwise, a temp window will be created with a gl context
HWND CreateOpenGLWindow( const HWND wnd, HGLRC & context );

// determines if an extension is supported
bool IsExtensionSupported( const char * const pExtension );

// different versions of gl that can be returned by GetVersion
extern const uint32_t VERSION_1_0;
extern const uint32_t VERSION_1_1;
extern const uint32_t VERSION_2_0;
extern const uint32_t VERSION_2_1;
extern const uint32_t VERSION_3_0;
extern const uint32_t VERSION_3_1;
extern const uint32_t VERSION_3_2;
extern const uint32_t VERSION_3_3;
extern const uint32_t VERSION_4_0;
extern const uint32_t VERSION_4_1;
extern const uint32_t VERSION_4_2;
extern const uint32_t VERSION_4_3;
extern const uint32_t VERSION_4_4;

// returns the current version of opengl
uint32_t GetVersion( );

}; // namespace gl

#endif // _OPENGL_EXTENSIONS_H_
