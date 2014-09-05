#ifndef _READ_TEXTURE_H_
#define _READ_TEXTURE_H_

// platform includes
#include "Window.h"

// gl includes
#include "GL/glew.h"
#include <GL/GL.h>

// std includes
#include <memory>
#include <cstdint>

// reads a rgb / rgba file
// assumes RGBA and UNSIGNED BYTE
bool ReadRGB( const char * const pFilename,
              uint32_t & width,
              uint32_t & height,
              std::shared_ptr< uint8_t > & pTexBuffer );

// reads a texture file
// assumes RGBA and UNSIGNED BYTE
bool ReadTexture( const char * const pFilename,
                  uint32_t & width,
                  uint32_t & height,
                  std::shared_ptr< uint8_t > & pTexBuffer );

// reads a texture file
template < typename T >
bool ReadTexture( const char * const pFilename,
                  const GLenum format,
                  uint32_t & width,
                  uint32_t & height,
                  std::shared_ptr< T > & pTexBuffer );

#endif // _READ_TEXTURE_H_
